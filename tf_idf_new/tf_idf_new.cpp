#define _CRT_SECURE_NO_WARNINGS
#define NUM 100

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct idfStruct {
	char *word;     //単語
	int df;     //df
	float idfWeight;     // idf
	struct idfStruct *nextAddr;     //次の構造体のアドレス
}idfStruct;

// tf-idf 用
typedef struct tfStruct {
	char *word;     //単語
	int tf;     //出てきた回数
	float tfWeight;     // tf
	float tfidfWeight;     // tf-idf
	struct tfStruct *nextAddr;     //次の構造体のアドレス
}tfStruct;

int main(int argc, char **argv)
{
	FILE *file;     //ファイルポインタ
	FILE *outputfile;     //ファイルポインタ
	char fileName[512] = "";    //ファイル名を格納するための配列
	char outputFileName[512] = "";    //ファイル名を格納するための配列

	idfStruct *idfwordsThis;    //リスト構造で今指している構造体のアドレス
	idfStruct *idfwordsList = NULL;
	idfStruct *idfwordsNew;

	tfStruct *tfwordsThis;    //リスト構造で今指している構造体のアドレス
	tfStruct *tfwordsList;
	tfStruct *tfwordsNew;

	tfStruct *wordsTemp;     //入れ替えなどの時に一時的に構造体のアドレスをもつ
	tfStruct *wordsPre;     //wordsThis のアドレスを持っている構造体のアドレスをもつ(交換用の仮置き場)
	tfStruct *wordsNext;    //今指している構造体の次のアドレスをもつ

	char *newWord;

	int filenum;     //ファイル番号

	int listLength = 0;     //リストの長さ


	//　↓　idfのリスト作成　↓
	sprintf(fileName, "%s", "..\\data\\df_new.txt");
	if ((file = fopen(fileName, "r")) == NULL)
	{
		printf("読み込みファイルが開けません\n");
		exit(EXIT_FAILURE);
	}

	char word[256];
	int dfVol = 0;
	float idfWeight = 0;

	while (fscanf(file, "%s\t%d\t%f\n", word, &dfVol, &idfWeight) != EOF)
	{
		newWord = (char *)malloc(sizeof(char) * (strlen(word) + 1));
		strcpy(newWord, word);

		idfwordsThis = idfwordsList;
		while (1)
		{
			if (idfwordsThis == NULL)//まだ 1 件の登録もない状態
			{
				break;
			}
			else if (strcmp(idfwordsThis->word, newWord) == 0)//取り出した語とリスト内の語が一致したとき
			{
				break;
			}
			//リストをたどるためのチェック
			if (idfwordsThis->nextAddr == NULL)//末尾を指しているとき
			{
				break;
			}
			else//次のアドレスがあるので，次のリンクに進む
			{
				idfwordsThis = idfwordsThis->nextAddr;
			}
		}

		//新たな語を保存するための新たな構造体領域を確保する（malloc）
		idfwordsNew = (idfStruct *)malloc(sizeof(idfStruct));
		//構造体に語を保存する
		idfwordsNew->word = newWord;
		//値のセット
		idfwordsNew->df = dfVol;
		idfwordsNew->idfWeight = idfWeight;
		//この構造体が末尾になるので，この次の構造体のアドレスを NULL にセットする
		idfwordsNew->nextAddr = NULL;

		//これが初めてのリストである時
		if (idfwordsList == NULL)
		{
			//リストの先頭を指すポインタ変数に，新しく作った構造体のアドレスを代入する
			idfwordsList = idfwordsNew;
		}
		else//すでにあるリストに追加するとき
		{
			//末尾の構造体のアドレス部分に，新しく作った構造体のアドレスを代入する
			idfwordsThis->nextAddr = idfwordsNew;
		}
	}
	//　↑　idfのリスト作成　↑

	//　↓	tfのリスト作成　↓
	for (filenum = 1; filenum <= 100; filenum++)
	{
		sprintf(fileName, "%s%03d%s", "..\\data\\tf_new_", filenum, ".txt");
		if ((file = fopen(fileName, "r")) == NULL)
		{
			printf("読み込みファイルが開けません\n");
			exit(EXIT_FAILURE);
		}

		sprintf(outputFileName, "%s%03d%s", "..\\data\\tfidf_new_", filenum, ".txt");
		if ((outputfile = fopen(outputFileName, "w")) == NULL)
		{
			printf("書き込みファイルが開けません。\n");
			exit(EXIT_FAILURE);
		}

		char word[256];
		int tfVol = 0;
		float tfWeight = 0;
		tfwordsList = NULL;


		while (fscanf(file, "%s\t%d\t%f\n", word, &tfVol, &tfWeight) != EOF)
		{
			newWord = (char *)malloc(sizeof(char) * (strlen(word) + 1));
			strcpy(newWord, word);

			tfwordsThis = tfwordsList;
			while (1)
			{
				if (tfwordsThis == NULL)//まだ 1 件の登録もない状態
				{
					break;
				}
				//リストをたどるためのチェック
				if (tfwordsThis->nextAddr == NULL)//末尾を指しているとき
				{
					break;
				}
				else//次のアドレスがあるので，次のリンクに進む
				{
					tfwordsThis = tfwordsThis->nextAddr;
				}
			}

			//新たな語を保存するための新たな構造体領域を確保する（malloc）
			tfwordsNew = (tfStruct *)malloc(sizeof(tfStruct) * 1);
			//構造体に語を保存する
			tfwordsNew->word = newWord;
			//構造体に値を保存する
			tfwordsNew->tf = tfVol;
			tfwordsNew->tfWeight = tfWeight;
			//この構造体が末尾になるので，この次の構造体のアドレスを NULL にセットする
			tfwordsNew->nextAddr = NULL;

			//これが初めてのリストである時
			if (tfwordsList == NULL)
			{
				//リストの先頭を指すポインタ変数に，新しく作った構造体のアドレスを代入する
				tfwordsList = tfwordsNew;
			}
			else//すでにあるリストに追加するとき
			{
				//末尾の構造体のアドレス部分に，新しく作った構造体のアドレスを代入する
				tfwordsThis->nextAddr = tfwordsNew;
			}
		}

		//　↓　tfとidfの掛け合わせ　↓
		tfwordsThis = tfwordsList;
		while (1) {
			idfwordsThis = idfwordsList;
			while (1)
			{
				if (strcmp(tfwordsThis->word, idfwordsThis->word) == 0)//取り出した語とリスト内の語が一致したとき
				{
					break;
				}
				else//次のアドレスがあるので，次のリンクに進む
				{
					idfwordsThis = idfwordsThis->nextAddr;
				}
			}

			tfwordsThis->tfidfWeight = (tfwordsThis->tfWeight) * (idfwordsThis->idfWeight);

			if (tfwordsThis->nextAddr == NULL)
				break;
			else
				tfwordsThis = tfwordsThis->nextAddr;
		}
		//　↑　tfとidfの掛け合わせ　↑

		//　↓　ソート　↓
		//リストの長さを測定
		tfwordsThis = tfwordsList;
		while (1)
		{
			listLength++;
			if (tfwordsThis->nextAddr == NULL)
			{
				//リストの末尾であるので，ループを抜ける
				break;
			}
			else
			{
				//指し示すアドレスを次に進める
				tfwordsThis = tfwordsThis->nextAddr;
			}
		}

		for (int i = 0; i < listLength; i++)
		{
			if (tfwordsList != NULL)
			{
				tfwordsThis = tfwordsList;
				wordsPre = NULL;

				while (1)
				{
					if (tfwordsThis->nextAddr != NULL)
					{
						if ((tfwordsThis->tfidfWeight) < (tfwordsThis->nextAddr->tfidfWeight))
						{
							wordsTemp = tfwordsThis;     //小さい方のアドレスを保管
							tfwordsThis = wordsTemp->nextAddr;
							wordsTemp->nextAddr = tfwordsThis->nextAddr;
							tfwordsThis->nextAddr = wordsTemp;

							if (wordsPre == NULL)//交換するオブジェクトが先頭だったら
							{
								tfwordsList = tfwordsThis;
							}
							else
							{
								//交換後に上位になるオブジェクトを前のオブジェクトのアドレスに登録する
								wordsPre->nextAddr = tfwordsThis;
							}
							wordsPre = tfwordsThis;     //ひとつ前の構造体のアドレスを更新する
							tfwordsThis = tfwordsThis->nextAddr;     //ここで参照する構造体のアドレスを更新する
						}
						else
						{
							wordsPre = tfwordsThis;     //ひとつ前の構造体のアドレスを更新する
							tfwordsThis = tfwordsThis->nextAddr;     //ここで参照する構造体のアドレスを更新する
						}
					}
					else
					{
						break;
					}
				}
			}
		}
		//　↑　ソート　↑

		//　↓　ファイル書き込み　↓
		if (tfwordsList->word != NULL)
		{
			tfwordsThis = tfwordsList;
			while (1)
			{
				fprintf(outputfile, "%s\t%d\t%f\t%f\n", tfwordsThis->word, tfwordsThis->tf, tfwordsThis->tfWeight, tfwordsThis->tfidfWeight);
				if (tfwordsThis->nextAddr == NULL)
				{
					//リストの末尾であるので，ループを抜ける
					break;
				}
				else
				{
					//指し示すアドレスを次に進める
					tfwordsThis = tfwordsThis->nextAddr;
				}
			}
		}
		//　↑　ファイル書き込み　↑

		//　↓　リスト領域の解放　↓
		if (tfwordsList != NULL)
		{
			tfwordsThis = tfwordsList;

			while (tfwordsThis != NULL)
			{
				wordsNext = tfwordsThis->nextAddr;     //(1)次の構造体のアドレスを取り出す
				free(tfwordsThis->word);     //(2)今指している構造体に含まれる語のメモリを解放する
				free(tfwordsThis);     //(3)今指している構造体を解放する
				tfwordsThis = wordsNext;     //(4)今指している構造体のアドレスを更新する
			}
		}
		//　↑　リスト領域の解放　↑
	}
	//　↑　tfのリスト作成　↑
	return 0;
}