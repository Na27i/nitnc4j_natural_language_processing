#define _CRT_SECURE_NO_WARNINGS
#define NUM 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct wordStruct {
	char *word;     //単語
	int count;     //出てきた回数
	struct wordStruct *nextAddr;     //次の構造体のアドレス
}wordStruct;

struct wordStruct *wordsTemp;     //入れ替えなどの時に一時的に構造体のアドレスをもつ
struct wordStruct *wordsPre;     //wordsThis のアドレスを持っている構造体のアドレスをもつ(交換用の仮置き場)
struct wordStruct *wordsNext;    //今指している構造体の次のアドレスをもつ

int main(int argc, char **argv)
{
	FILE *file;     //ファイルポインタ(読みこみ)
	FILE *outputFile;     //ファイルポインタ(書きこみ)
	char fileName[512] = "";    //ファイル名を格納するための配列
	char outputFileName[512] = "";    //ファイル名を格納するための配列

	sprintf(outputFileName, "%sdf_new%s", "..\\data\\", ".txt");
	if ((outputFile = fopen(outputFileName, "w")) == NULL)
	{
		printf("書き込みファイルが開けません。\n");
		exit(EXIT_FAILURE);
	}

	int foundFlag;
	wordStruct *wordsThis;    //リスト構造で今指している構造体のアドレス
	wordStruct *wordsList = NULL;
	wordStruct *wordsNew;

	char *newWord;

	int filenum;     //ファイル番号

	int listLength = 0;     //リストの長さ

	for (filenum = 1; filenum <= 100; filenum++)
	{
		sprintf(fileName, "%s%03d%s", "..\\data\\tf_new_", filenum, ".txt");
		if ((file = fopen(fileName, "r")) == NULL)
		{
			printf("読み込みファイルが開けません\n");
			exit(EXIT_FAILURE);
		}

		char word[256];
		float tfVol = 0;
		float tfWeight = 0;

		while (fscanf(file, "%s\t%f\t%f\n", word, &tfVol, &tfWeight) != EOF)
		{
			printf("%03d:%s\n", filenum, word);
			newWord = (char *)malloc(sizeof(char) * (strlen(word) + 1));
			strcpy(newWord, word);

			foundFlag = 0;
			wordsThis = wordsList;
			while (1)
			{
				if (wordsThis == NULL)//まだ 1 件の登録もない状態
				{
					break;
				}
				else if (strcmp(wordsThis->word, newWord) == 0)//取り出した語とリスト内の語が一致したとき
				{
					foundFlag = 1;
					break;
				}
				//リストをたどるためのチェック
				if (wordsThis->nextAddr == NULL)//末尾を指しているとき
				{
					break;
				}
				else//次のアドレスがあるので，次のリンクに進む
				{
					wordsThis = wordsThis->nextAddr;
				}
			}

			if (foundFlag == 1)//リストに語があったので，その語のカウントを増やす
			{
				wordsThis->count++;
				free(newWord);//この語はすでにリストにあるので，とっておく必要がないので解放する
			}
			else//リストになかったので，単語を追加する．
			{
				//新たな語を保存するための新たな構造体領域を確保する（malloc）
				wordsNew = (wordStruct *)malloc(sizeof(wordStruct) * 1);
				//構造体に語を保存する
				wordsNew->word = newWord;
				//語の出現回数を 1 にセットする
				wordsNew->count = 1;
				//この構造体が末尾になるので，この次の構造体のアドレスを NULL にセットする
				wordsNew->nextAddr = NULL;

				//これが初めてのリストである時
				if (wordsList == NULL)
				{
					//リストの先頭を指すポインタ変数に，新しく作った構造体のアドレスを代入する
					wordsList = wordsNew;
				}
				else//すでにあるリストに追加するとき
				{
					//末尾の構造体のアドレス部分に，新しく作った構造体のアドレスを代入する
					wordsThis->nextAddr = wordsNew;
				}
			}
		}
	}

	//リストの長さを測定
	while (1)
	{
		listLength++;
		if (wordsThis->nextAddr == NULL)
		{
			//リストの末尾であるので，ループを抜ける
			break;
		}
		else
		{
			//指し示すアドレスを次に進める
			wordsThis = wordsThis->nextAddr;
		}
	}

	//　↓　ソート
	for (int i = 0; i < listLength; i++)
	{
		if (wordsList != NULL)
		{
			wordsThis = wordsList;
			wordsPre = NULL;

			while (1)
			{
				if (wordsThis->nextAddr != NULL)
				{
					if (wordsThis->count < wordsThis->nextAddr->count)
					{
						wordsTemp = wordsThis;     //小さい方のアドレスを保管
						wordsThis = wordsTemp->nextAddr;
						wordsTemp->nextAddr = wordsThis->nextAddr;
						wordsThis->nextAddr = wordsTemp;

						if (wordsPre == NULL)//交換するオブジェクトが先頭だったら
						{
							wordsList = wordsThis;
						}
						else
						{
							//交換後に上位になるオブジェクトを前のオブジェクトのアドレスに登録する
							wordsPre->nextAddr = wordsThis;
						}
						wordsPre = wordsThis;     //ひとつ前の構造体のアドレスを更新する
						wordsThis = wordsThis->nextAddr;     //ここで参照する構造体のアドレスを更新する

					}
					else
					{
						wordsPre = wordsThis;     //ひとつ前の構造体のアドレスを更新する
						wordsThis = wordsThis->nextAddr;     //ここで参照する構造体のアドレスを更新する
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	//　↑　ソート

	//　↓　書き込み
	if (wordsList->word != NULL)
	{
		wordsThis = wordsList;
		while (1)
		{
			fprintf(outputFile, "%s\t%d\t%f\n", wordsThis->word, wordsThis->count, (log10(NUM / wordsThis->count)) + 1);
			if (wordsThis->nextAddr == NULL)
			{
				//リストの末尾であるので，ループを抜ける
				break;
			}
			else
			{
				//指し示すアドレスを次に進める
				wordsThis = wordsThis->nextAddr;
			}
		}
	}
	// ↑　書き込み

	return 0;
}