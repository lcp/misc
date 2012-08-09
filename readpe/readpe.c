#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

#include "pe.h"

int
print_32plus_header (data_directory *datadir, struct pe32plus_opt_hdr *pe)
{
	printf ("text size:      %d\n", pe->text_size);
	printf ("data size:      %d\n", pe->data_size);
	printf ("bss  size:      %d\n", pe->bss_size);
	printf ("entry point:    %d\n", pe->entry_point);
	printf ("code base:      %d\n", pe->code_base);
	printf ("image base:     %lld\n", pe->image_base);
	printf ("section align:  %d\n", pe->section_align);
	printf ("file align:     %d\n", pe->file_align);
	printf ("image size:     %d\n", pe->image_size);
	printf ("header size:    %d\n", pe->header_size);
	printf ("stack size req: %lld\n", pe->stack_size_req);
	printf ("stack size:     %lld\n", pe->stack_size);
	printf ("heap size req:  %lld\n", pe->heap_size_req);
	printf ("heap size:      %lld\n", pe->heap_size);

	printf ("\n== datadir ==\n");
	printf ("exports size:   %d\n", datadir->exports.size);
	printf ("imports size:   %d\n", datadir->imports.size);
	printf ("resources size: %d\n", datadir->resources.size);
	printf ("exceptions size:%d\n", datadir->exceptions.size);
	printf ("cert size:      %d\n", datadir->certs.size);
}

int
read_header (void *data)
{
	struct mz_hdr *mz_hdr;
	struct pe_hdr *pe_hdr;
	struct pe32_opt_hdr *pe_o_hdr;
	off_t hdr;
	data_directory *dd;

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

	hdr = mz_hdr->peaddr;
	pe_o_hdr = (struct pe32_opt_hdr *)(data + hdr + sizeof(*pe_hdr));

	switch (pe_o_hdr->magic) {
		case PE_OPT_MAGIC_PE32:
			printf ("== pe32 ==\n");
			break;
		case PE_OPT_MAGIC_PE32PLUS:
			printf ("== pe32+ ==\n");
			dd = (data_directory *)(data + hdr + sizeof(*pe_hdr) + sizeof (struct pe32plus_opt_hdr));
			print_32plus_header (dd, (struct pe32plus_opt_hdr *)pe_o_hdr);
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
