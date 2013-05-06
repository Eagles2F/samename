#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "libcsv.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

#define THRESHOLD 0
#define LINENUMBER 247207

class MyPool
{
public:
	MyPool()
	{
		m_Cnt = 0;
	}
public:
	int m_Cnt;
	int m_Id[10];
};// record the result of one line

int Id[LINENUMBER] = {0};	//AuthorID
char* Name[LINENUMBER] = {0};	//Author Name
char* Affiliation[LINENUMBER] = {0};	//Affiliation
int flag = 1;	//count variable
int IsFirstLine = 1;
int Cnt = 0;	//count variable
int AuthorNumber = 0;
vector<MyPool> SameName;	//record the result using a vector

void hlog (void *data, size_t len, void *t)
{
	//第一行不读
	if(IsFirstLine)
	{
		return;
	}
	char *str = (char*)data;
	//将数据存入相应的结构中
	switch(flag)
	{
	case 1:
		{
			char *pData = (char*)malloc(sizeof(char)*(len + 1));
			strncpy(pData,(char*)data,len);
			pData[len] = 0;
			Id[Cnt] = atoi(pData);
			free(pData);
			flag ++;
			break;
		}
	case 2:
		{
			Name[Cnt] = (char*)malloc(sizeof(char)*len);
			strncpy(Name[Cnt],(char*)data,len);
			Name[Cnt][len] = '\0';
			flag ++;
			break;
		}
	default:
		{
			Affiliation[Cnt] = (char*)malloc(sizeof(char)*len);
			strncpy(Affiliation[Cnt],(char*)data,len);
			Affiliation[Cnt][len] = '\0';
			break;
		}
	}
}

void nlog (int c, void *t)
{
	if(IsFirstLine)
	{
		IsFirstLine = 0;
		return;
	}
	flag = 1;
	Cnt ++;
	AuthorNumber ++;
}


int main (int argc,char *argv[]) {
	FILE *fp;

	int t = 0;
	struct csv_parser *p;
	char buf[1024];
	size_t bytes_read;
	if((p=(struct csv_parser*)malloc(sizeof(struct csv_parser))) == 0) return -1;
	csv_init(p, (unsigned char)0);
	fp = (argc == 1) ? fopen("C:/Users/Eagles2F/Desktop/DionyBuddy/data/dataRev2/Author.csv","r") : fopen(argv[1], "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Failed to open %s\n", argv[1]);
	}


	while ((bytes_read=fread(buf, 1, 1024, fp)) > 0)
	{
		if (csv_parse(p, buf, bytes_read, hlog, nlog,&t) != bytes_read)
		{
			fprintf(stderr, "Error while parsing file: %s\n", csv_strerror(csv_error(p)));
		}
	}
    fclose(fp);
	for(int i = 0;i < AuthorNumber;i ++)
	{
		MyPool *pMP = new MyPool;
		SameName.push_back(*pMP);
		SameName[SameName.size() - 1].m_Id[SameName[SameName.size() - 1].m_Cnt] = Id[i];
		SameName[SameName.size() - 1].m_Cnt ++;

		const char* p = Name[i];
		for(int j = 0;j < AuthorNumber;j ++)
		{
			const char* q = Name[j];
			//比较名字是否相同
			if(strcmp(p,q) == 0)
			{
				//名字为空默认为只是与自己相同
				if(p[0] == 0 && i != j)
				{
					continue;
				}
				SameName[SameName.size() - 1].m_Id[SameName[SameName.size() - 1].m_Cnt] = Id[j];
				SameName[SameName.size() - 1].m_Cnt ++;
			}
		}
		if(i%1000 == 1)
		{
			printf(".");
		}
	}

	FILE *pFile = fopen("Submission.csv","w");
	fprintf(pFile,"AuthorId,DuplicateAuthorIds\n");
	for(int i = 0;i < AuthorNumber;i ++)
	{
		fprintf(pFile,"%d,",SameName[i].m_Id[0]);
		for(int j = 1;j < SameName[i].m_Cnt - 1;j ++)
		{
			fprintf(pFile,"%d ",SameName[i].m_Id[j]);
		}
		fprintf(pFile,"%d\n",SameName[i].m_Id[SameName[i].m_Cnt - 1]);
	}
    fclose(pFile);
	getchar();

	return EXIT_SUCCESS;
}


