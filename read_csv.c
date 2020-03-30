#define NDEBUG

#include "dbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct name
{
	char* first_name;
	char* last_name;
} name;

typedef struct employee
{
	name name;
	char* job_title;
	char* phone;
	double salary;
} employee;

void remove_char(char *s, char c);
int highest_salary(struct employee e[], int number_of_rows);
void display_employees(employee e[], int number_of_rows);
void get_file_attributes(FILE* fp, int* number_of_rows, int* max_char_per_row);

int main(int argc, char* argv[])
{
    assert(argc >= 3);
    char* fp_input = argv[1];
    int has_header = atoi(argv[2]);

    FILE* fp = fopen(fp_input, "r");
    int number_of_rows = 0;
    int max_char_per_row = 0;

    /* get file attributes will traverse the entire buffer simply to get row and
     * row length. it would be better to use auto growing number_of_rows and
     * max_char_per_row.
    */
    get_file_attributes(fp, &number_of_rows, &max_char_per_row);
    debug("number of rows: %d", number_of_rows);
    debug("chars per rows: %d", max_char_per_row);

	employee employees[number_of_rows];
    char line[max_char_per_row];
    char element[max_char_per_row];

    int in_quotes = 0;
    int column_number = 0;
    int line_pos = 0;
    int line_number = 0;

    rewind(fp);

    while (fgets(line, max_char_per_row, fp))
    {
        debug("line: %s", line);

        if (line_number == 0 && has_header)
        {
            line_number++;
            continue;
        }

        column_number = 0;

        for (int cursor = 0; line[cursor]; cursor++)
        {
            debug("cursor: %d", cursor);
            debug("line[cursor]: %c", line[cursor]);

            if (line[cursor] == '"')
            {
                in_quotes = !in_quotes;
                continue;
            }

            element[line_pos++] = line[cursor]; //build a word until we find a terminator

            if (!in_quotes && (line[cursor] == ',' || line[cursor] == '\n'))
            {
                element[line_pos - 1] = 0; // replace comma or new line with 0
                line_pos = 0;


                switch (column_number)
                {
                    case 0:;
                        char* first_name = calloc(strlen(element) + 1, sizeof(char));
                        strcpy(first_name, element);
                        employees[line_number].name.first_name = first_name;
                        break;

                    case 1:
                        break;

                    case 2:;
                        char* last_name = calloc(strlen(element) + 1, sizeof(char));
                        strcpy(last_name, element);
                        employees[line_number].name.last_name = last_name;
                        break;

                    case 3:;
                        char* phone = calloc(strlen(element) + 1, sizeof(char));
                        strcpy(phone, element);
                        employees[line_number].phone = phone;
                        break;

                    case 4:;
                        char* job_title = calloc(strlen(element) + 1, sizeof(char));
                        strcpy(job_title, element);
                        employees[line_number].job_title = job_title;
                        break;

                    case 5:
                        remove_char(element, ',');
                        employees[line_number].salary = atof(element);
                        break;
                    default:
                        sentinel("not implemented");
                }
                column_number++; //valid element found
            }
        }
        line_number++;
        debug("line_number: %d", line_number);
    }

    display_employees(employees, line_number);
    printf("%s %s makes the most money!\n\n",
            employees[highest_salary(employees, line_number)].name.first_name,
            employees[highest_salary(employees, line_number)].name.last_name);


    for (int f = 1; f < line_number; f++)
    {
        free(employees[f].name.first_name);
        free(employees[f].name.last_name);
        free(employees[f].job_title);
        free(employees[f].phone);
    }
    fclose(fp);

    return 0;

error:
    if (fp)
        fclose(fp);
    return -1;

}


void display_employees(employee e[], int number_of_rows)
{
    for (int i = 1; i < number_of_rows; i++)
    {
        printf("\n");
        printf("first_name: %s\n", e[i].name.first_name);
        printf("last_name: %s\n", e[i].name.last_name);
        printf("job_title: %s\n", e[i].job_title);
        printf("phone: %s\n", e[i].phone);
        printf("salary: %f\n", e[i].salary);
    }

    printf("\n");
}

int highest_salary(employee e[], int number_of_rows)
{
	int index;
	float highest;

	highest = e[1].salary;
	index = 1;
	for(int i = 1; i < number_of_rows; i++)
	{
		if(e[i].salary > highest)
		{
			highest = e[i].salary;
			index = i;
		}
	}

	return index;
}

void get_file_attributes(FILE* fp, int* number_of_rows, int* max_char_per_row)
{
    int number_char_per_row = 100000;
    char row[number_char_per_row];
    int rows = 0;
    int chars_per_row = 0;
    int char_count = 0;
    while (fgets(row, number_char_per_row, fp))
    {
        for (int c = 0; row[c]; c++)
        {
            char_count++;
        }
        if (char_count > chars_per_row)
        {
            chars_per_row = char_count;
        }
        char_count = 0;
        debug("max char count per row: %d\n", chars_per_row);
        rows++;
    }
    *number_of_rows = rows;
    *max_char_per_row = chars_per_row + 1;
}

void remove_char(char *s, char c)
{
   int i, j = 0;
   for ( i = 0; s[i] != '\0'; i++ )
   {
      if ( s[i] != c )
      {
         s[j++] = s[i]; // j only moves after we write a non char
      }
   }
   s[j] = '\0';
}

