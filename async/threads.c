#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void* read_file(void* p_arg)
{
	const char* filename = (const char*)p_arg;
	FILE* file = fopen(filename, "r");
	if (file == NULL)
		return NULL;

	size_t len = 0;
	char* data = (char*)malloc((len + 1) * sizeof(char));
	if (data == NULL)
	{
		fclose(file);
		return NULL;
	}

	data[0] = '\0';

	char buffer[127 + 1];
	while (fgets(buffer, sizeof(buffer) / sizeof(char), file) != NULL)
	{
		size_t new_len = len + strlen(buffer);
		char* new_data = (char*)malloc((new_len + 1) * sizeof(char));
		if (new_data == NULL)
		{
			free(data);
			fclose(file);
			return NULL;
		}

		/* copy the data acquired previously */
		strcpy(new_data, data);
		/* fill up the rest of the place starting from the
		 * trailing zero of the previous copy operation */
		strcpy(new_data + len, buffer);

		len = new_len;
		free(data);
		data = new_data;
	}

	fclose(file);
	return data;
}

int main(int argc, char* argv[])
{
	pthread_t t_io;
	pthread_create(&t_io, NULL, read_file, "data.txt");

	printf("We're doing some good shit on the main thread tho.\n");

	char* data;
	int success = pthread_join(t_io, (void**)&data);

	if (success == 0 && data != NULL)
	{
		puts(data);
		puts("And that sums it up. :)");
		free(data);
	}

	return 0;
}
