#define _CRT_SECURE_NO_WARNINGS
#define NUM 100

#include "mecab.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// df-idf 用
typedef struct idfStruct {
	char *word;     //単語
	int df;     //出てきた回数
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
	FILE *file;     //ファイルポインタ(読みこみ)
	char idfFileName[512] = "..\\data\\idf.txt";    //ファイル名を格納するための配列
	char tfFileName[512] = "";    //ファイル名を格納するための配列

	idfStruct *idfwordsThis;    //リスト構造で今指している構造体のアドレス
	idfStruct *idfwordsList = NULL;
	idfStruct *idfwordsNew;

	tfStruct *tfwordsThis;    //リスト構造で今指している構造体のアドレス
	tfStruct *tfwordsList = NULL;
	tfStruct *tfwordsNew;

	char *newWord;

	int filenum;     //ファイル番号
	int listLength = 0;     //リストの長さ

	if ((file = fopen(idfFileName, "r")) == NULL)
	{
		printf("読み込みファイルが開けません\n");
		exit(EXIT_FAILURE);
	}

	char word[256];
	int idfVol;
	float idfWeight;

	while (fscanf(file, "%s\t%d\t%f\n", word, &idfVol, &idfWeight) != EOF)
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
		idfwordsNew = (idfStruct *)malloc(sizeof(idfStruct) * 1);
		//構造体に語を保存する
		idfwordsNew->word = word;
		//値のセット
		idfwordsNew->df = idfVol;
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






	for (filenum = 1; filenum <= 100; filenum++)
	{
		sprintf(tfFileName, "%s%03d%s", "..\\data\\tf_", filenum, ".txt");
		if ((file = fopen(tfFileName, "r")) == NULL)
		{
			printf("読み込みファイルが開けません\n");
			exit(EXIT_FAILURE);
		}

		int tfVol = 0;
		float tfWeight = 0;

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
				else if (strcmp(tfwordsThis->word, newWord) == 0)//取り出した語とリスト内の語が一致したとき
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
			//値のセット
			tfwordsNew->tf = tfVol;
			tfwordsNew->tfWeight = tfWeight;
			idfwordsThis = idfwordsList;
			while (1) {
				if (strcmp(tfwordsNew->word, idfwordsThis->word) != 0) {
						//指し示すアドレスを次に進める
						idfwordsThis = idfwordsThis->nextAddr;
				}
				else{
					tfwordsNew->tfidfWeight = tfwordsNew->tfWeight * idfwordsThis->idfWeight;
				}
			}
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
	}













	return 0;
}