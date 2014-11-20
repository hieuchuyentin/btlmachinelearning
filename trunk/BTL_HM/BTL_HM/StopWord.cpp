#include "stdafx.h"
#include "StopWord.h"
#include "Stemmer.h"
StopWord::StopWord()
{
	readStopWords();
}

bool operator == (TOKEN_INFO i1, TOKEN_INFO i2)
{
	return (i1.sTokenName.Compare(i2.sTokenName) == 0);
}
StopWord::~StopWord()
{
}
void StopWord::readStopWords()
{
	CString words[] = {
		_T("a"), _T("a's"), _T("able"), _T("about"), _T("above"), _T("according"), _T("accordingly"), _T("across"), _T("actually"), _T("after"), _T("afterwards"),
		_T("again"), _T("against"), _T("ain't"), _T("all"), _T("allow"), _T("allows"), _T("almost"), _T("alone"), _T("along"), _T("already"), _T("also"), _T("although"),
		_T("always"), _T("am"), _T("among"), _T("amongst"), _T("an"), _T("and"), _T("another"), _T("any"), _T("anybody"), _T("anyhow"), _T("anyone"), _T("anything"), _T("anyway"),
		_T("anyways"), _T("anywhere"), _T("apart"), _T("appear"), _T("appreciate"), _T("appropriate"), _T("are", "aren't"), _T("around"), _T("as"), _T("aside"), _T("ask"),
		_T("asking"), _T("associated"), _T("at"), _T("available"), _T("away"), _T("awfully"),
		_T("b"), _T("be"), _T("became"), _T("because"), _T("become"), _T("becomes"), _T("becoming"), _T("been"), _T("before"), _T("beforehand"), _T("behind"), _T("being"),
		_T("believe"), _T("below"), _T("beside"), _T("besides"), _T("best"), _T("better"), _T("between"), _T("beyond"), _T("both"), _T("brief"), _T("but"), _T("by"),
		_T("c"), _T("c'mon"), _T("c's"), _T("came"), _T("can"), _T("can't"), _T("cannot"), _T("cant"), _T("cause"), _T("causes"), _T("certain"), _T("certainly"), _T("changes"),
		_T("clearly"), _T("co"), _T("com"), _T("come"), _T("comes"), _T("concerning"), _T("consequently"), _T("consider"), _T("considering"), _T("contain"),
		_T("containing"), _T("contains"), _T("corresponding"), _T("could"), _T("couldn't"), _T("course"), _T("currently"),
		_T("d"), _T("definitely"), _T("described"), _T("despite"), _T("did"), _T("didn't"), _T("different"), _T("do"), _T("does"), _T("doesn't"), _T("doing"),
		_T("don't"), _T("done"), _T("down"), _T("downwards"), _T("during"),
		_T("e"), _T("each"), _T("edu"), _T("eg"), _T("eight"), _T("either"), _T("else"), _T("elsewhere"), _T("enough"), _T("entirely"), _T("especially"),
		_T("et"), _T("etc"), _T("even"), _T("ever"), _T("every"), _T("everybody"), _T("everyone"), _T("everything"), _T("everywhere"), _T("ex"),
		_T("exactly"), _T("example"), _T("except"),
		_T("f"), _T("far"), _T("few"), _T("fifth"), _T("first"), _T("five"), _T("followed"), _T("following"), _T("follows"), _T("for"), _T("former"), _T("formerly"), _T("forth"),
		_T("four"), _T("from"), _T("further"), _T("furthermore"),
		_T("g"), _T("get"), _T("gets"), _T("getting"), _T("given"), _T("gives"), _T("go"), _T("goes"), _T("going"), _T("gone"), _T("got"), _T("gotten"), _T("greetings"),
		_T("h"), _T("had"), _T("hadn't"), _T("happens"), _T("hardly"), _T("has"), _T("hasn't"), _T("have"), _T("haven't"), _T("having"), _T("he"), _T("he's"),
		_T("hello"), _T("help"), _T("hence"), _T("her"), _T("here"), _T("here's"), _T("hereafter"), _T("hereby"), _T("herein"), _T("hereupon"),
		_T("hers"), _T("herself"), _T("hi"), _T("him"), _T("himself"), _T("his"), _T("hither"), _T("hopefully"), _T("how"), _T("howbeit"), _T("however"),
		_T("i"), _T("i'd"), _T("i'll"), _T("i'm"), _T("i've"), _T("ie"), _T("if"), _T("ignored"), _T("immediate"), _T("in"), _T("inasmuch"), _T("inc"), _T("indeed"),
		_T("indicate"), _T("indicated"), _T("indicates"), _T("inner"), _T("insofar"), _T("instead"), _T("into"), _T("inward"), _T("is"), _T("isn't"),
		_T("it"), _T("it'd"), _T("it'll"), _T("it's"), _T("its"), _T("itself"),
		_T("j"), _T("just"),
		_T("k"), _T("keep"), _T("keeps"), _T("kept"), _T("know"), _T("known"), _T("knows"),
		_T("l"), _T("last"), _T("lately"), _T("later"), _T("latter"), _T("latterly"), _T("least"), _T("less"), _T("lest"), _T("let"), _T("let's"), _T("like"), _T("liked"),
		_T("likely"), _T("little"), _T("look"), _T("looking"), _T("looks"), _T("ltd"),
		_T("m"), _T("mainly"), _T("many"), _T("may"), _T("maybe"), _T("me"), _T("mean"), _T("meanwhile"), _T("merely"), _T("might"),
		_T("more"), _T("moreover"), _T("most"), _T("mostly"), _T("much"), _T("must"), _T("my"), _T("myself"),
		_T("n"), _T("name"), _T("namely"), _T("nd"), _T("near"), _T("nearly"), _T("necessary"), _T("need"), _T("needs"), _T("neither"), _T("never"), _T("nevertheless"), _T("new"),
		_T("next"), _T("nine"), _T("no"), _T("nobody"), _T("non"), _T("none"), _T("noone"), _T("nor"), _T("normally"), _T("not"), _T("nothing"), _T("novel"), _T("now"), _T("nowhere"),
		_T("o"), _T("obviously"), _T("of"), _T("off"), _T("often"), _T("oh"), _T("ok"), _T("okay"), _T("old"), _T("on"), _T("once"), _T("one"),
		_T("ones"), _T("only"), _T("onto"), _T("or"), _T("other"), _T("others"), _T("otherwise"), _T("ought"), _T("our"), _T("ours"),
		_T("ourselves"), _T("out"), _T("outside"), _T("over"), _T("overall"), _T("own"),
		_T("p"), _T("particular"), _T("particularly"), _T("per"), _T("perhaps"), _T("placed"), _T("please"), _T("plus"), _T("possible"), _T("presumably"),
		_T("probably"), _T("provides"),
		_T("q"), _T("que"), _T("quite"), _T("qv"),
		_T("r"), _T("rather"), _T("rd"), _T("re"), _T("really"), _T("reasonably"), _T("regarding"), _T("regardless"), _T("regards"), _T("relatively"), _T("respectively"),
		_T("right"),
		_T("s"), _T("said"), _T("same"), _T("saw"), _T("say"), _T("saying"), _T("says"), _T("second"), _T("secondly"), _T("see"), _T("seeing"), _T("seem"), _T("seemed"),
		_T("seeming"), _T("seems"), _T("seen"), _T("self"), _T("selves"), _T("sensible"), _T("sent"), _T("serious"), _T("seriously"), _T("seven"), _T("several"), _T("shall"),
		_T("she"), _T("should"), _T("shouldn't"), _T("since"), _T("six"), _T("so"), _T("some"), _T("somebody"), _T("somehow"), _T("someone"), _T("something"), _T("sometime"),
		_T("sometimes"), _T("somewhat"), _T("somewhere"), _T("soon"), _T("sorry"), _T("specified"), _T("specify"), _T("specifying"), _T("still"), _T("sub"), _T("such"),
		_T("sup"), _T("sure"),
		_T("t"), _T("t's"), _T("take"), _T("taken"), _T("tell"), _T("tends"), _T("th"), _T("than"), _T("thank"), _T("thanks"), _T("thanx"), _T("that"), _T("that's"), _T("thats"), _T("the"),
		_T("their"), _T("theirs"), _T("them"), _T("themselves"), _T("then"), _T("thence"), _T("there"), _T("there's"), _T("thereafter"), _T("thereby"), _T("therefore"),
		_T("therein"), _T("theres"), _T("thereupon"), _T("these"), _T("they"), _T("they'd"), _T("they'll"), _T("they're"), _T("they've"), _T("think"), _T("third"), _T("this"),
		_T("thorough"), _T("thoroughly"), _T("those"), _T("though"), _T("three"), _T("through"), _T("throughout"), _T("thru"), _T("thus"), _T("to"), _T("together"), _T("too"),
		_T("took"), _T("toward"), _T("towards"), _T("tried"), _T("tries"), _T("truly"), _T("try"), _T("trying"), _T("twice"), _T("two"),
		_T("u"), _T("un"), _T("under"), _T("unfortunately"), _T("unless"), _T("unlikely"), _T("until"), _T("unto"), _T("up"), _T("upon"), _T("us"), _T("use"), _T("used"), _T("useful"), _T("uses"), _T("using"), _T("usually"),
		_T("v"), _T("value"), _T("various"), _T("very"), _T("via"), _T("viz"), _T("vs"),
		_T("w"), _T("want"), _T("wants"), _T("was"), _T("wasn't"), _T("way"), _T("we"), _T("we'd"), _T("we'll"), _T("we're"), _T("we've"), _T("welcome"), _T("well"), _T("went"), _T("were"),
		_T("weren't"), _T("what"), _T("what's"), _T("whatever"), _T("when"), _T("whence"), _T("whenever"), _T("where"), _T("where's"), _T("whereafter"),
		_T("whereas"), _T("whereby"), _T("wherein"), _T("whereupon"), _T("wherever"), _T("whether"), _T("which"), _T("while"), _T("whither"), _T("who"),
		_T("who's"), _T("whoever"), _T("whole"), _T("whom"), _T("whose"), _T("why"), _T("will"), _T("willing"), _T("wish"), _T("with"), _T("within"), _T("without"),
		_T("won't"), _T("wonder"), _T("would"), _T("wouldn't"),
		_T("y"), _T("yes"), _T("yet"), _T("you"), _T("you'd"), _T("you'll"), _T("you're"), _T("you've"), _T("your"), _T("yours"), _T("yourself"), _T("yourselves"),
		_T("z"), _T("zero") };
	int h = sizeof(words) / sizeof(*words);
	for (int i = 0; i< h; i++)
	{
		listStopWord.AddTail(words[i]);
	}	
}
void StopWord::ProcessStopWord(CStringList& listMail, TOKEN_LIST& listToken)
{
	CString restoken;
	Stemmer stem = Stemmer();
	int cusPos;

	listToken.RemoveAll();
	
	for (int i = 0; i < listMail.GetCount(); i++){
		CString mail = listMail.GetAt(listMail.FindIndex(i));
		mail.Replace('\b', ' ');
		mail.Replace('\t', ' ');
		mail.Replace('\n', ' ');
		mail.Replace('\f', ' ');
		mail.Replace('\r', ' ');
		mail.Replace(',', ' ');
		mail.Replace('!', ' ');
		mail.Replace(':', ' ');
		mail.Replace(';', ' ');
		mail.Replace('?', ' ');
		mail.Replace('.', ' ');
		mail.Replace('[', ' ');
		mail.Replace(']', ' ');
		mail.Replace('(', ' ');
		mail.Replace(')', ' ');
		mail.Replace('{', ' ');
		mail.Replace('}', ' ');
		cusPos = 0;
		mail.MakeLower();
		restoken = mail.Tokenize(_T(" "), cusPos);
		while (restoken != _T(""))
		{
			if (listStopWord.Find(restoken) == NULL){
				stem.resetIndex();
				stem.stemmer(restoken);
				TOKEN_INFO s;
				s.sTokenName = stem.toString();
				s.dPercent = 1;
				POSITION p = listToken.Find(s);
				if (p == NULL)
				{
					listToken.AddTail(s);
				}
				else
				{
					s.dPercent = listToken.GetAt(p).dPercent + 1;
// 					listToken.RemoveAt(p);
// 					listToken.AddTail(s);
					listToken.SetAt(p, s);//Hieu
				}
			}
			restoken = mail.Tokenize(_T(" "), cusPos);
		}
	}
}