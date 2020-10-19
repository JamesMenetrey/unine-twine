#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv)
{
    char buff[100] = { '\0' };
    FILE* file = fopen("data.txt", "w+");

    if (file == NULL) {
        printf("Could not open the file, error: %d.\n", errno);
        return -1;
    }
    
    fprintf(file, "world");

    fseek(file, 0, SEEK_SET);
    fscanf(file, "%s", buff);
    printf("Read value: %s\n", buff);

    long cursor_position = ftell(file);
    printf("Cursor position: %ld\n", cursor_position);

    int flushed_result = fflush(file);
    printf("Flushed result: %d\n", flushed_result);

    fclose(file);

    return 0;
}
