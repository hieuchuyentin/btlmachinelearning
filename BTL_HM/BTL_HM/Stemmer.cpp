#include "stdafx.h"
#include "Stemmer.h"


Stemmer::Stemmer()
{
}


Stemmer::~Stemmer()
{
}
void Stemmer::resetIndex()
{
	delete[] b;
	char * new_b = new char[INC];
	*b = *new_b;
	i = 0;
	i_end = 0;
	j = 0;
	k = 0;
}
void Stemmer::add(char ch)
{
	if (i == (sizeof(b) / sizeof(char)))
	{
		char * new_b = new char[i + INC];
		for (int c = 0; c < i; c++)
		{
			new_b[c] = b[c];
		}
		delete[] b;
		*b = *new_b;
	}
	b[i++] = ch;
}
void Stemmer::add(char w[], int wLen)
{
	if (i + wLen >= (sizeof(b) / sizeof(char)))
	{
		char * new_b = new char[i + wLen + INC];
		for (int c = 0; c < i; c++) new_b[c] = b[c];
		delete[] b;
		*b = *new_b;
	}
	for (int c = 0; c < wLen; c++) b[i++] = w[c];
}
CString Stemmer::toString()
{
	char * new_b = new char[i_end];
	for (int c = 0; c < i; c++)
	{
		new_b[c] = b[c];
	}
	return CString(new_b);
}
int Stemmer::getResultLength()
{
	return i_end;
}
char *Stemmer::getResultBuffer()
{
	return b;
}
bool Stemmer::cons(int i)
{
	switch (b[i])
	{
	case 'a': case 'e': case 'i': case 'o': case 'u': return false;
	case 'y': return (i == 0) ? true : !cons(i - 1);
	default: return true;
	}
}
int Stemmer::m()
{
	int n = 0;
	int i = 0;
	while (true)
	{
		if (i > j) return n;
		if (!cons(i)) break; i++;
	}
	i++;
	while (true)
	{
		while (true)
		{
			if (i > j) return n;
			if (cons(i)) break;
			i++;
		}
		i++;
		n++;
		while (true)
		{
			if (i > j) return n;
			if (!cons(i)) break;
			i++;
		}
		i++;
	}
}
bool Stemmer::vowelinstem()
{
	int i;
	for (i = 0; i <= j; i++)
		if (!cons(i)) return true;
	return false;
}
bool Stemmer::doublec(int j)
{
	if (j < 1) return false;
	if (b[j] != b[j - 1]) return false;
	return cons(j);
}
bool Stemmer::cvc(int i)
{
	if (i < 2 || !cons(i) || cons(i - 1) || !cons(i - 2)) return false;
	{
		int ch = b[i];
		if (ch == 'w' || ch == 'x' || ch == 'y') return false;
	}
	return true;
}
bool Stemmer::ends(CString s)
{
	int l = s.GetLength();
	int o = k - l + 1;
	if (o < 0) return false;
	for (int i = 0; i < l; i++) if (b[o + i] != s.GetAt(i)) return false;
	j = k - l;
	return true;
}
void Stemmer::setto(CString s)
{
	int l = s.GetLength();
	int o = j + 1;
	for (int i = 0; i < l; i++) b[o + i] = s.GetAt(i);
	k = j + l;
}
void Stemmer::r(CString s)
{
	if (m() > 0) setto(s);
}
void Stemmer::step1()
{

	if (b[k] == 's')
	{
		if (ends(_T("sses")))
			k -= 2;
		else if (ends(_T("ies")))
			setto(_T("i"));
		else if (b[k - 1] != 's')
			k--;
	}
	if (ends(_T("eed")))
	{
		if (m() > 0) k--;
	}
	else if ((ends(_T("ed")) || ends(_T("ing"))) && vowelinstem())
	{
		k = j;
		if (ends(_T("at")))
			setto(_T("ate"));
		else if (ends(_T("bl")))
			setto(_T("ble"));
		else if (ends(_T("iz")))
			setto(_T("ize"));
		else if (doublec(k))
		{
			k--;
			{
				int ch = b[k];
				if (ch == 'l' || ch == 's' || ch == 'z')
					k++;
			}
		}
		else if (m() == 1 && cvc(k))
			setto(_T("e"));
	}
}
void Stemmer::step2()
{
	if (ends(_T("y")) && vowelinstem())
		b[k] = 'i';
}
void Stemmer::step3()
{
	if (k == 0) return;
	switch (b[k - 1])
	{
	case 'a':
		if (ends(_T("ational")))
		{
			r(_T("ate"));
			break;
		}
		if (ends(_T("tional")))
		{
			r(_T("tion"));
			break;
		}
		break;
	case 'c':
		if (ends(_T("enci")))
		{
			r(_T("ence"));
			break;
		}
		if (ends(_T("anci")))
		{
			r(_T("ance"));
			break;
		}
		break;
	case 'e':
		if (ends(_T("izer")))
		{
			r(_T("ize"));
			break;
		}
		break;
	case 'l':
		if (ends(_T("bli")))
		{
			r(_T("ble"));
			break;
		}
		if (ends(_T("alli")))
		{
			r(_T("al"));
			break;
		}
		if (ends(_T("entli")))
		{
			r(_T("ent"));
			break;
		}
		if (ends(_T("eli")))
		{
			r(_T("e"));
			break;
		}
		if (ends(_T("ousli")))
		{
			r(_T("ous"));
			break;
		}
		break;
	case 'o':
		if (ends(_T("ization")))
		{
			r(_T("ize"));
			break;
		}
		if (ends(_T("ation")))
		{
			r(_T("ate"));
			break;
		}
		if (ends(_T("ator")))
		{
			r(_T("ate"));
			break;
		}
		break;
	case 's':
		if (ends(_T("alism")))
		{
			r(_T("al"));
			break;
		}
		if (ends(_T("iveness")))
		{
			r(_T("ive"));
			break;
		}
		if (ends(_T("fulness")))
		{
			r(_T("ful"));
			break;
		}
		if (ends(_T("ousness")))
		{
			r(_T("ous"));
			break;
		}
		break;
	case 't':
		if (ends(_T("aliti")))
		{
			r(_T("al"));
			break;
		}
		if (ends(_T("iviti")))
		{
			r(_T("ive"));
			break;
		}
		if (ends(_T("biliti")))
		{
			r(_T("ble"));
			break;
		}
		break;
	case 'g':
		if (ends(_T("logi")))
		{
			r(_T("log"));
			break;
		}
	}
}
void Stemmer::step4()
{
	switch (b[k])
	{
	case 'e':
		if (ends(_T("icate")))
		{
			r(_T("ic"));
			break;
		}
		if (ends(_T("ative")))
		{
			r(_T(""));
			break;
		}
		if (ends(_T("alize")))
		{
			r(_T("al"));
			break;
		}
		break;
	case 'i':
		if (ends(_T("iciti")))
		{
			r(_T("ic"));
			break;
		}
		break;
	case 'l':
		if (ends(_T("ical")))
		{
			r(_T("ic"));
			break;
		}
		if (ends(_T("ful")))
		{
			r(_T(""));
			break;
		}
		break;
	case 's':
		if (ends(_T("ness")))
		{
			r(_T(""));
			break;
		}
		break;
	}
}
void Stemmer::step5()
{
	if (k == 0) {
		return;
	}
	switch (b[k - 1])
	{
	case 'a':
		if (ends(_T("al")))
		{
			break;
		}
		return;
	case 'c':
		if (ends(_T("ance")))
		{
			break;
		}
		if (ends(_T("ence")))
		{
			break;
		}
		return;
	case 'e':
		if (ends(_T("er")))
		{
			break;
		}
		return;
	case 'i':
		if (ends(_T("ic")))
		{
			break;
		}
		return;
	case 'l':
		if (ends(_T("able")))
		{
			break;
		}
		if (ends(_T("ible")))
		{
			break;
		}
		return;
	case 'n':
		if (ends(_T("ant")))
		{
			break;
		}
		if (ends(_T("ement")))
		{
			break;
		}
		if (ends(_T("ment")))
		{
			break;
		}
		if (ends(_T("ent")))
		{
			break;
		}
		return;
	case 'o':
		if (ends(_T("ion")) && j >= 0 && (b[j] == 's' || b[j] == 't'))
		{
			break;
		}
		if (ends(_T("ou")))
		{
			break;
		}
		return;
	case 's':
		if (ends(_T("ism")))
		{
			break;
		}
		return;
	case 't':
		if (ends(_T("ate")))
		{
			break;
		}
		if (ends(_T("iti")))
		{
			break;
		}
		return;
	case 'u':
		if (ends(_T("ous")))
		{
			break;
		}
		return;
	case 'v':
		if (ends(_T("ive")))
		{
			break;
		}
		return;
	case 'z':
		if (ends(_T("ize")))
		{
			break;
		}
		return;
	default:
		return;
	}
	if (m() > 1) {
		k = j;
	}
}
void Stemmer::step6()
{
	j = k;
	if (b[k] == 'e')
	{
		int a = m();
		if (a > 1 || a == 1 && !cvc(k - 1))
			k--;
	}
	if (b[k] == 'l' && doublec(k) && m() > 1)
		k--;
}
void Stemmer::createArrayChar(CString s)
{
	char* new_b = new char[s.GetLength()];
	for (int c = 0; c < s.GetLength(); c++)
	{
		new_b[c] = s.GetAt(c);
	}
	add(new_b, s.GetLength());
	delete[] new_b;
}
void Stemmer::stemmer(CString s)
{
	createArrayChar(s);
	k = i - 1;
	if (k > 1)
	{
		step1();
		step2();
		step3();
		step4();
		step5();
		step6();
	}
	i_end = k + 1; i = 0;
}
