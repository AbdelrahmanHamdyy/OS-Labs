#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

bool JKP(char *s1, char *s2)
{
    int l1 = strlen(s1);
    int l2 = strlen(s2);

    // Reject equal size strings
    if (l1 == l2)
        return 0;

    // Remove Case Sensitivity
    for (int i = 0; i < l1; i++)
        s1[i] = tolower(s1[i]);
    for (int i = 0; i < l2; i++)
        s2[i] = tolower(s2[i]);

    // Determine the longer string
    int maxL = l1 > l2 ? l1 : l2;
    char* maxStr = l1 > l2 ? s1 : s2;
    char* minStr = l1 < l2 ? s1 : s2;
    int minL = l1 < l2 ? l1 : l2;

    // Check
    int* arr = malloc(minL * sizeof(int));
    for (int i = 0; i < minL; i++)
    {
        arr[i] = 0;
        bool found = false;
        for (int j = 0; j < maxL; j++)
        {
            if (minStr[i] == maxStr[j])
            {
                if (i == 0 || arr[i - 1] < j)
                {
                    found = true;
                    arr[i] = j;
                    break;
                }
            }
        }
        if (!found)
            return 0;
    }
    return 1;
}

int main(int argc, char **argv) {

    if (argc < 3) {	 
		printf("Too few arguments. Exiting!\n");
		exit(1);
	}

    if (JKP(argv[1], argv[2]))
        puts("1");
    else
        puts("0");

    return 0;
}