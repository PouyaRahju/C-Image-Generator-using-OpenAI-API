#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include "include/curl/curl.h"
#include "include/json-c/json.h"

/* Function to download generated images */
int Download(char *url);

/* variables to save response of image generator requests in buffer */
char *responseData;
int responseData_Size = 1;
/* to know first callback */
int isfirst = 1;

/*  image generator WRITEFUNCTION callback to recv json data */
size_t callback(char *response, size_t size, size_t nmemb, void *custom)
{
    size_t len = size * nmemb;
    responseData_Size += len;
    responseData = (char *)realloc(responseData, responseData_Size);

    if (isfirst)
    {
        isfirst = 0;
        strcpy(responseData, response);
    }
    else
    {
        strcat(responseData, response);
    }

    return len;
}

/* image generator  wants a prompt to generate images with OpenAI API */
int GenerateImage(char *prompt)
{

    CURLcode ret;
    CURL *hnd;
    struct curl_slist *slist1;

    struct json_object *data, *newData, *dataInsideJson, *another_json, *url;

    char image_address[500];

    responseData = (char *)malloc(responseData_Size);

    data = json_object_new_object();

    /* Prompt to generate images */
    /* Like:    a baby smoking  */
    newData = json_object_new_string(prompt);
    json_object_object_add(data, "prompt", newData);

    /* Number of images */
    /* You can set number of images will generate */
    newData = json_object_new_int(1);
    json_object_object_add(data, "n", newData);

    /*  Size of image */
    /*  The size of image can be 256x256, 512x512 and 1024x1024 */
    newData = json_object_new_string("256x256");
    json_object_object_add(data, "size", newData);

    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");
    slist1 = curl_slist_append(slist1, "Authorization: Bearer {OPEN_AI_API}");

    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL, "https://api.openai.com/v1/images/generations");
    curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, json_object_get_string(data));
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(json_object_get_string(data)));
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.83.1");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callback);

    ret = curl_easy_perform(hnd);

    curl_easy_cleanup(hnd);
    hnd = NULL;
    curl_slist_free_all(slist1);
    slist1 = NULL;

    data = json_tokener_parse(responseData);

    dataInsideJson = json_object_object_get(data, "data");

    another_json = json_object_array_get_idx(dataInsideJson, 0);

    url = json_object_object_get(another_json, "url");

    strcpy(image_address, json_object_get_string(url));

    isfirst = 1;
    Download(image_address);

    free(responseData);
    responseData_Size = 1;

    return (int)ret;
}
/* Convert int to string function */
void tostring(char *str, int num)
{
    int i, rem, len = 0, n;

    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

FILE *dfp;

int filecount = 1;/* the first part of filename.jpg */
size_t Download_callback(char *response, size_t size, size_t nmemb, void *custom)
{
    size_t len = size * nmemb;
    /* write image file data */
    fwrite(response, len, 1, dfp);

    return len;
}

/* Image downloader */
int Download(char *url)
{

    CURLcode ret;
    CURL *hnd;
    char filename[100];
    /* open image file to save downloaded data image */
    memset(filename, '\0', 100);
    tostring(filename, filecount);
    strcat(filename, ".jpg");
    printf("%s\n", filename);
    dfp = fopen(filename, "wb");
    filecount++;




    hnd = curl_easy_init();
    curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(hnd, CURLOPT_URL, url);
    curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.83.1");
    curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, Download_callback);

    ret = curl_easy_perform(hnd);

    curl_easy_cleanup(hnd);
    hnd = NULL;

    fclose(dfp);

    return (int)ret;
}

#endif