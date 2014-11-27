#pragma once
#include "Stemmer.h"
typedef struct TOKEN_INFO
{
	CString sTokenName;
	double dPercent;
} TOKEN_INFO;
typedef CList<TOKEN_INFO, TOKEN_INFO&> TOKEN_LIST;

typedef CList<int, int&> NUMBER_LIST;

class ProcessWord
{
public:
	ProcessWord();
	~ProcessWord();
	void readStopWords();
	void PreProcessListMail(CStringList& listMail);
	void DeleteStopWord(TOKEN_LIST listToken[], int nHashCount);
	void StemMailList(CStringList& listMail);
	CList<CString> listStopWord;

	//TOKEN_LIST buffer;//Danh sach tu  va so lan suat hien
private:
	
	
};

