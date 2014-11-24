#pragma once
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
	void StopAndStemWord(CStringList& listMail);
	CList<CString> listStopWord;
	//TOKEN_LIST buffer;//Danh sach tu  va so lan suat hien
private:
	
	
};

