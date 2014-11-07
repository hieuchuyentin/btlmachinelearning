#pragma once
#define INC 50 /* kich thuoc cua mang char stemmer*/
class Stemmer
{
public:
	Stemmer();
	~Stemmer();
	void resetIndex();
	void add(char ch);
	void add(char w[], int wLen);
	CString toString();
	int getResultLength();
	char* getResultBuffer();
	bool cons(int i);
	int m();
	bool vowelinstem();
	bool doublec(int j);
	bool cvc(int i);
	bool ends(CString s);
	void setto(CString s);
	void r(CString s);
	void step1();
	void step2();
	void step3();
	void step4();
	void step5();
	void step6();
	void stemmer(CString s);
	void createArrayChar(CString s);
private:
	char b[INC];
	int  i, i_end,j, k;
	
};

