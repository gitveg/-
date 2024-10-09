#include <curl/curl.h>    
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json11.hpp"

//����POST������صĽṹ��ͺ���
typedef struct {
    std::string body;
    size_t bodySize;
} stResponse;

typedef struct {
    std::string header;
    size_t headerSize;
} stResponseHeader;

size_t responseBodyCallback(void* ptr, size_t size, size_t nmemb, void* stream) {
    stResponse* pResponse = (stResponse*)stream;
    pResponse->body.append((char*)ptr, size * nmemb);
    pResponse->bodySize = size * nmemb;
    return size * nmemb;
}

size_t responseHeaderCallback(void* ptr, size_t size, size_t nmemb, void* stream) {
    stResponseHeader* pResponseHeader = (stResponseHeader*)stream;
    pResponseHeader->header.append((char*)ptr, size * nmemb);
    pResponseHeader->headerSize = size * nmemb;
    return size * nmemb;
}
//��������������������������������������������������������������������

//��������
void http_post_generate(char post_data[5096], json11::Json::array* context);
void http_post_chat(char post_data[5096]);

//������ʵ�ֵĹ��ܣ��ܹ��м���Ľ��жԻ�
int main() {
    std::cout << "Put 1 to exit" << std::endl;
    char postData[5096];    //�洢POST�Ĳ�������
    std::string propt_str;  //�洢�ͻ���Ҫ���ģ��ѯ�ʵ�����
    json11::Json::array context;
    json11::Json post_body; //JSON��ʽ��POST��������

    while (1) {
        std::getline(std::cin, propt_str);
        if (!strcmp(propt_str.c_str(), "1"))break;
        //JSON����Ķ�������ڸ���prpt_str֮����Ϊ�ⶨ���Ϊconst
        post_body = json11::Json::object{
		    { "model", "llama3.1" },    //ѡ��Ĵ�ģ��
		    { "prompt", propt_str},    //���ģ��ѯ�ʵ�����
		    { "stream", false },        //�Է���ʽ����������Ĳ�����
			{"context",context},        //Ϊ��ʵ�ֶ��ݵĶԻ�����
        };
        strcpy_s(postData, post_body.dump().c_str());
        std::cout << postData << std::endl;
        http_post_generate(postData,&context);
    }




    return 0;
}



void http_post_generate(char post_data[5096], json11::Json::array* context) {       //generate API�ĵ��ú�����context����������ݵĶԻ�����
    std::string readBuffer;
    stResponse response;
    stResponseHeader responseHeader;

    // ��ʼ�����п��ܵĵ���
    curl_global_init(CURL_GLOBAL_ALL);


    CURL* curl = curl_easy_init();

    // ����url
    curl_easy_setopt(curl, CURLOPT_URL, "http://222.20.126.129:11434/api/generate");

    // ����post���󣬲����û�����Ϊ0��Ϊget����
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // ����post������
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    // ����post�������С
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_data));

    // ����http����ͷ
    curl_slist* headerList = NULL;
    headerList = curl_slist_append(headerList, "Content-Type: application/json");
    headerList = curl_slist_append(headerList, "flag: libcurl");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);

    // ���ò�У��֤�飬https����ʱʹ��
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

    // ���ûص�������ȡ��Ӧ������
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseBodyCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

    // ���ûص�������ȡ��Ӧͷ����
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, responseHeaderCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*)&responseHeader);

    // ��ʱʱ��
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);

    // ִ������
    CURLcode res = curl_easy_perform(curl);

    // ������
    if (res == CURLE_OK) {
        // ��ȡ״̬��
        int responseCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        std::cout << "code : " << responseCode << std::endl;

        //������Ϣ
        //std::cout << "responseHeader size : " << responseHeader.headerSize << std::endl;
        //std::cout << "responseHeader header : " << responseHeader.header.c_str() << std::endl;

        //std::cout << "response size : " << response.bodySize << std::endl;
        //std::cout << "response body : " << response.body.c_str() << std::endl;

        //��response bodyת��ΪJSON����
        std::string err;
        json11::Json mJson = json11::Json::parse(response.body.c_str(), err);

        std::cout << "AI response :" << mJson["response"].string_value()<<std::endl;
        *context = mJson["context"].array_items();
    }
    else {
        std::cout << curl_easy_strerror(res) << std::endl;
    }

    curl_slist_free_all(headerList);

    // ����
    curl_easy_cleanup(curl);
}

void http_post_chat(char post_data[5096]) { //��ʱ�ò����Ľӿ�
    std::string readBuffer;
    stResponse response;
    stResponseHeader responseHeader;

    // ��ʼ�����п��ܵĵ���
    curl_global_init(CURL_GLOBAL_ALL);


    CURL* curl = curl_easy_init();

    // ����url
    curl_easy_setopt(curl, CURLOPT_URL, "http://222.20.126.129:11434/api/chat");

    // ����post���󣬲����û�����Ϊ0��Ϊget����
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    // ����post������
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    // ����post�������С
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_data));

    // ����http����ͷ
    curl_slist* headerList = NULL;
    headerList = curl_slist_append(headerList, "Content-Type: application/json");
    headerList = curl_slist_append(headerList, "flag: libcurl");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);

    // ���ò�У��֤�飬https����ʱʹ��
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

    // ���ûص�������ȡ��Ӧ������
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseBodyCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

    // ���ûص�������ȡ��Ӧͷ����
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, responseHeaderCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void*)&responseHeader);

    // ��ʱʱ��
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);

    // ִ������
    CURLcode res = curl_easy_perform(curl);

    // ������
    if (res == CURLE_OK) {
        // ��ȡ״̬��
        int responseCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        std::cout << "code : " << responseCode << std::endl;

        //������Ϣ
        //std::cout << "responseHeader size : " << responseHeader.headerSize << std::endl;
        //std::cout << "responseHeader header : " << responseHeader.header.c_str() << std::endl;

        //std::cout << "response size : " << response.bodySize << std::endl;
        //std::cout << "response body : " << response.body.c_str() << std::endl;

        //��response bodyת��ΪJSON����
        std::string err;
        json11::Json mJson = json11::Json::parse(response.body.c_str(), err);

        std::cout << "AI response :" << mJson["response"].string_value();
    }
    else {
        std::cout << curl_easy_strerror(res) << std::endl;
    }

    curl_slist_free_all(headerList);

    // ����
    curl_easy_cleanup(curl);
}
