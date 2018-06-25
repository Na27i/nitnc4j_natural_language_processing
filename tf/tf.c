#define _CRT_SECURE_NO_WARNINGS

#include "mecab.h"
#include <stdio.h>
#include <stdlib.h>

#define CHECK(eval) if (! eval) { \
    fprintf (stderr, "Exception:%s\n", mecab_strerror (mecab)); \
    mecab_destroy(mecab); \
    return -1; }


typedef struct wordStruct {
	char *word;     //単語
	int count;     //出てきた回数
	struct wordStruct *nextAddr;     //次の構造体のアドレス
}wordStruct;

struct wordStruct *wordsTemp;     //入れ替えなどの時に一時的に構造体のアドレスをもつ
struct wordStruct *wordsPre;     //wordsThis のアドレスを持っている構造体のアドレスをもつ(交換用の仮置き場)
struct wordStruct *wordsNext;    //今指している構造体の次のアドレスをもつ


int main(int argc, char **argv) {
	FILE *file;     //ファイルポインタ
	int fileSize;     //ファイルのサイズ
	char fileName[512] = "";    //ファイル名を格納するための配列

	char *string;     //文字列を格納するための配列
	char ch;     //roop用
	int i;     //roop用

	char *word;     //ファイルから取り出した文字列

	int foundFlag = 0;     //
	wordStruct *wordsThis;    //リスト構造で今指している構造体のアドレス
	wordStruct *wordsList;     //
	wordStruct *wordsNew;

	mecab_t *mecab;
	const mecab_node_t *node;

	int filenum;

	for (filenum = 1; filenum <= 100; filenum++)
	{
		sprintf(fileName, "%s%03d%s", "..\\data\\", filenum, ".txt");
		if ((file = fopen(fileName, "r")) == NULL)
		{
			printf("file open error!!\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			fseek(file, 0, SEEK_END);     //ファイルポインタを末尾に移動させる
			fileSize = ftell(file);     //ファイルサイズを取得する
			printf("%s ファイルのサイズ:%dバイト\n", fileName, fileSize);
			fseek(file, 0, SEEK_SET);     //ファイルポインタを先頭に戻す
		}



		FILE *outputFile;
		char outputFileName[32] = "";
		sprintf(outputFileName, "%s%03d%s", "..\\data\\tf_", filenum, ".txt");
		if ((outputFile = fopen(outputFileName, "w")) == NULL)
		{
			printf("ファイルが開けません", outputFileName);
			exit;
		}



		wordsList = NULL;

		string = (char *)malloc(sizeof(char) * (fileSize + 1));
		for (i = 0; (ch = fgetc(file)) != -1; i++)
		{
			string[i] = ch;
		}
		string[i] = '\0';

		mecab = mecab_new(argc, argv);
		CHECK(mecab);
		// Gets node object
		node = mecab_sparse_tonode(mecab, string);
		CHECK(node);
		for (; node; node = node->next)
		{
			if (node->stat == MECAB_NOR_NODE || node->stat == MECAB_UNK_NODE)
			{
				fwrite(node->surface, sizeof(char), node->length, stdout);
				printf("\t%s\n", node->feature);
				word = (char *)malloc(sizeof(char) * (node->length + 1));
				strncpy(word, node->surface, node->length);
				word[node->length] = '\0';

				foundFlag = 0;
				wordsThis = wordsList;
				while (1)
				{
					if (wordsThis == NULL)//まだ 1 件の登録もない状態
					{
						break;
					}
					else if (strcmp(wordsThis->word, word) == 0)//取り出した語とリスト内の語が一致したとき
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
					free(word);//この語はすでにリストにあるので，とっておく必要がないので解放する
				}
				else//リストになかったので，単語を追加する．
				{
					//新たな語を保存するための新たな構造体領域を確保する（malloc）
					wordsNew = (wordStruct *)malloc(sizeof(wordStruct) * 1);
					//構造体に語を保存する
					wordsNew->word = word;
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
		mecab_destroy(mecab);

		int listLength = 0;
		int wordsVolume = 0;

		//リストの長さを測定
		while (1)
		{
			listLength++;
			wordsVolume += wordsThis->count;
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

		listLength = 0;

		if (wordsList->word != NULL)
		{
			wordsThis = wordsList;
			while (1)
			{
				printf("%s:%d:%f\n", wordsThis->word, wordsThis->count, (float)(wordsThis->count) / wordsVolume);
				fprintf(outputFile, "%s\t%d\t%f\n", wordsThis->word, wordsThis->count, (float)(wordsThis->count) / wordsVolume);
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

		//リスト領域を解放する
		if (wordsList != NULL)
		{
			wordsThis = wordsList;

			while (wordsThis != NULL)
			{
				wordsNext = wordsThis->nextAddr;     //(1)次の構造体のアドレスを取り出す
				free(wordsThis->word);     //(2)今指している構造体に含まれる語のメモリを解放する
				free(wordsThis);     //(3)今指している構造体を解放する
				wordsThis = wordsNext;     //(4)今指している構造体のアドレスを更新する
			}
		}
	}
	return 0;
}
