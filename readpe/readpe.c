#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

#include "pe.h"

int
print_32plus_header (struct pe_hdr *pe_hdr, struct pe32plus_opt_hdr *pe32plus)
{
	data_directory *datadir;
	struct section_header *shdr, *scn_ptr;
	size_t ddsize = 0;
	int i, j;

	ddsize = pe32plus->data_dirs;

	datadir = (data_directory *)((char *)pe32plus + sizeof (struct pe32plus_opt_hdr));
	shdr = (struct section_header *)((char *)datadir + (sizeof (data_dirent) * ddsize));

	printf ("text size:      %d\n", pe32plus->text_size);
	printf ("data size:      %d\n", pe32plus->data_size);
	printf ("bss  size:      %d\n", pe32plus->bss_size);
	printf ("entry point:    %d\n", pe32plus->entry_point);
	printf ("code base:      %d\n", pe32plus->code_base);
	printf ("image base:     %lld\n", pe32plus->image_base);
	printf ("section align:  %d\n", pe32plus->section_align);
	printf ("file align:     %d\n", pe32plus->file_align);
	printf ("image size:     %d\n", pe32plus->image_size);
	printf ("header size:    %d\n", pe32plus->header_size);
	printf ("stack size req: %lld\n", pe32plus->stack_size_req);
	printf ("stack size:     %lld\n", pe32plus->stack_size);
	printf ("heap size req:  %lld\n", pe32plus->heap_size_req);
	printf ("heap size:      %lld\n", pe32plus->heap_size);
	printf ("data dirs:      %d\n", pe32plus->data_dirs);

	printf ("\n== datadir ==\n");
	printf ("exports size:   %d\n", datadir->exports.size);
	printf ("exports addr:   %lld\n", datadir->exports.virtual_address);
	printf ("imports size:   %d\n", datadir->imports.size);
	printf ("imports addr:   %lld\n", datadir->imports.virtual_address);
	printf ("resources size: %d\n", datadir->resources.size);
	printf ("resources addr: %lld\n", datadir->resources.virtual_address);
	printf ("except size:    %d\n", datadir->exceptions.size);
	printf ("except addr:    %lld\n", datadir->exceptions.virtual_address);
	printf ("cert size:      %d\n", datadir->certs.size);
	printf ("cert addr:      %lld\n", datadir->certs.virtual_address);
	printf ("reloc size:     %d\n", datadir->base_relocations.size);
	printf ("reloc addr:     %lld\n", datadir->base_relocations.virtual_address);

	printf ("\n== section header ==\n");
	scn_ptr = shdr;
	for (i = 0; i < pe_hdr->sections; i++) {
		printf ("Sect[%d] name:   ", i+1);
		for (j = 0; j < 8; j++)
			putchar (scn_ptr->name[j]);
		putchar ('\n');
		printf ("Sect[%d] v_size: %d\n", i+1, scn_ptr->virtual_size);
		printf ("Sect[%d] v_addr: %d\n", i+1, scn_ptr->virtual_address);
		putchar ('\n');
		scn_ptr += 1;
	}
}

int
read_header (void *data)
{
	struct mz_hdr *mz_hdr;
	struct pe_hdr *pe_hdr;
	struct pe32_opt_hdr *pe_o_hdr;

	mz_hdr = data;

	if (mz_hdr->magic != 0x5a4d) {
		fprintf (stderr, "Not MZ\n");
		return -1;
	}

	pe_hdr = (struct pe_hdr *)(data + mz_hdr->peaddr);

	if (pe_hdr->magic != 0x00004550) {
		fprintf (stderr, "Not PE\n");
		return -1;
	}

	pe_o_hdr = (struct pe32_opt_hdr *)((char *)pe_hdr + sizeof(struct pe_hdr));

	switch (pe_o_hdr->magic) {
		case PE_OPT_MAGIC_PE32:
			printf ("== pe32 ==\n");
			break;
		case PE_OPT_MAGIC_PE32PLUS:
			printf ("== pe32+ ==\n");
			print_32plus_header (pe_hdr, (struct pe32plus_opt_hdr *)pe_o_hdr);
			break;
		case PE_OPT_MAGIC_PE32_ROM:
			printf ("== pe32 ROM ==");
			break;
		default:
			printf ("unknown");
			return -1;
	}

	return 0;
}

int
main (int argc, char *argv[])
{
	char *filename;
	int input_fd;
	size_t length;
	struct stat st;
	void *data;

	if (argc < 2) {
		printf ("Usage: readpe <pecoff file>\n");
		return 1;
	}

	filename = argv[1];

	input_fd = open(filename, O_RDONLY|O_CLOEXEC);
	if (input_fd < 0) {
		fprintf (stderr, "Failed to open %s\n", filename);
		return 1;
	}

	if (fstat(input_fd, &st) == 0 &&
	    (sizeof(size_t) >= sizeof(st.st_size) ||
	    st.st_size <= ~((size_t)0)))
		length = (size_t) st.st_size;

	data = mmap (NULL, length, PROT_READ, MAP_SHARED, input_fd, 0);
	if (data == MAP_FAILED) {
		fprintf (stderr, "mmap failed\n");
		goto out;
	}

	read_header (data);

	munmap (data, length);
out:
	close (input_fd);

	return 0;
}
