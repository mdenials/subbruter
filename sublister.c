
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

int main (int argc, char *argv[])
{
    int d_flag = 0;
    int l_flag = 0;

    char dstring[256];
    char lstring[256];

    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) 
        {
            d_flag = 1;
            strncpy(dstring, argv[i+1], sizeof(dstring) - 1); // -1 to leave space for null terminator
            i++; // increment i to skip the argument value
        }

        if (strcmp(argv[i], "-l") == 0) 
        {
            l_flag = 1;
            strncpy(lstring, argv[i+1], sizeof(lstring) - 1); // -1 to leave space for null terminator
            i++; // increment i to skip the argument value
        }
    }

    printf("Domain: %s, List: %s\n", dstring, lstring);

    if (d_flag == 1 && l_flag == 1)
    {
        CURL *curl;
        CURLcode res;
        char status_code[4]; // buffer to store the status code
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        FILE *list;
        list = fopen(lstring, "r");
        if (list == NULL) {
            printf("Error opening file %s\n", lstring);
            return 1;
        }

        char line[256];
        while (fgets(line, sizeof(line), list) != NULL)
        {
            line[strcspn(line, "\n")] = '\0';
            char domain[256];
            strcpy(domain, line);
            strcat(domain, ".");
            strcat(domain, dstring);

            if (curl)
            {
                curl_easy_setopt(curl, CURLOPT_URL, domain);
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
                res = curl_easy_perform(curl);

                if(res == CURLE_OK)
                {
                    long http_code;
                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                    sprintf(status_code, "%ld", http_code);
                    printf("%s|%ld\n", domain, http_code);
                }
            }
        }
        fclose(list); // close the file
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}
