#include    <stdio.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <time.h>
#include    <string.h>
#include    <termio.h>


#define TRUE 1
#define FALSE 0
#define WORDS 1500
#define WORD_LEN 100
#define MINIMUM_LEN 5
#define MAX_FAILURE 7
#define LINE_MAX_LENGTH 500  //単語の抽出 一行の最大文字数
#define WORD_FILE "/home/yuma/Program/C/hangman/toeic1500.dat"


void getWord(char word[][WORD_LEN]);
int play(char word[]);
char getChar(void);
int displayAndDecision(char word[], char usedCharacter[], int remainingTimes);
void wordSelection(char word[],char wordList[][WORD_LEN]);
void result(int gameTimes, int failureTimes, int correctTimes,char failureWords[][WORD_LEN]);

int main(void){
    char wordList[WORDS][WORD_LEN];
    char word[WORD_LEN];
    int gameTimes,failureTimes,correctTimes;
    int isProceed = TRUE;
    int isCorrect;
    char failedWords[WORDS][WORD_LEN];
    char c;
    gameTimes = failureTimes = correctTimes = 0;

    srandom((unsigned int)time(NULL));
    getWord(wordList);

    while (isProceed) {
        wordSelection(word, wordList);
        isCorrect = play(word);
        gameTimes++;
        if (isCorrect){
            correctTimes++;
        } else {
            strcpy(failedWords[failureTimes],word);
            failureTimes++;
        }
        printf("\n続けますか？（y/n） >> ");
        c = getChar();
        printf("\n");
        if(c != 'y'){
            isProceed = FALSE;
        }
    }

    result(gameTimes, failureTimes, correctTimes,failedWords);

    return 0;
}

void getWord(char word[][WORD_LEN]){
    FILE *fp;
    char tmp[LINE_MAX_LENGTH];

    if ((fp = fopen(WORD_FILE,"r")) == NULL){
        fprintf(stderr,"FATAL:単語ファイルの読み込みに失敗しました:%s\n",WORD_FILE);
        exit(1);
    }

//    一行ずつ処理 英単語を抽出
    int i,j,k;
    j = 0;
    while (fgets(tmp, LINE_MAX_LENGTH, fp) != NULL){
        i=0;
        while (tmp[i] != '\0'){
            if ('a' <= tmp[i] && tmp[i] <= 'z'){
                k = 0;
                while ('a' <= tmp[i] && tmp[i] <= 'z') {
                    word[j][k] = tmp[i];
                    k++;
                    i++;
                }
                word[j][k] = '\0';
                j++;
            }
            i++;
        }
    }

    fclose(fp);

}

char getChar(void)
{
    struct termio old_term, new_term;

    char    c;

    ioctl(0, TCGETA, &old_term);
    new_term = old_term;
    //new_term.c_lflag &= ~(ICANON | ECHO);
    new_term.c_lflag &= ~(ICANON);
    ioctl(0, TCSETAW, &new_term);

    c = getchar();

    ioctl(0, TCSETAW, &old_term);

    return (c);
}

int play(char word[]){
    int remainingTimes = MAX_FAILURE;
    char usedCharacter[WORD_LEN];
    int isUsed = FALSE;
    int isCorrectAnswer = FALSE;
    int isReduceRemainingTimes = TRUE;
    int count = 0;
    char c;
    usedCharacter[0] = '\0';
    displayAndDecision(word, usedCharacter, remainingTimes);

    while (remainingTimes > 0){
        printf("文字を入力してください.");
        c = getChar();

//        英子文字以外受け付けない
        if (!('a' <= c && c <= 'z')){
            printf("\n英子文字を入力してください\n");
            continue;
        }

//        入力された文字がすでに使われていないか
        int i = 0;
        isUsed = FALSE;
        while (usedCharacter[i] != '\0') {
            if (usedCharacter[i] == c){
                isUsed = TRUE;
            }
            i++;
        }
        if (isUsed){
            printf("\nすでに使われている文字です\n");
            continue;
        }

//        残り回数を減らすかどうか
        i = 0;
        isReduceRemainingTimes = TRUE;
        while (word[i] != '\0'){
            if (word[i] == c){
                isReduceRemainingTimes = FALSE;
            }
            i++;
        }
        if (isReduceRemainingTimes){
            remainingTimes--;
        }

        usedCharacter[count] = c;
        usedCharacter[count + 1] = '\0';
        count++;

//        表示と正解の判定
        isCorrectAnswer = displayAndDecision(word, usedCharacter, remainingTimes);
        if (isCorrectAnswer){
            return TRUE;
            break;
        }
    }

//    正解できなかった場合
    if (!(isCorrectAnswer)) {
        printf("残念！正解は「%s」", word);
        return FALSE;
    }
}

int displayAndDecision(char word[], char usedCharacter[], int remainingTimes){
    int isCorrectAnswer = FALSE;
    int isCorrectAnswerAll = TRUE;

    printf("\n\n単語:");
    int i = 0;
    int j;
//    正解している文字は表示,それ以外は - を表示 また,全体を通して正解しているかもチェック
    while (word[i] != '\0'){
        j = 0;
        while (usedCharacter[j] != '\0') {
            if (word[i] == usedCharacter[j]){
                isCorrectAnswer = TRUE;
            }
            j++;
        }
        if (isCorrectAnswer){
            printf("%c",word[i]);
        } else {
            printf("-");
            isCorrectAnswerAll = FALSE;
        }
        isCorrectAnswer = FALSE;
        i++;
    }
    printf("\n");

//    remainingTimes!=7 初回表示時は正解としない
    if (isCorrectAnswerAll && remainingTimes != MAX_FAILURE) {
        printf("正解!!");
    } else {
        printf("使われた文字:%s\n", usedCharacter);
        printf("残り回数:%d\n", remainingTimes);
    }

    return isCorrectAnswerAll;
}

void wordSelection(char word[],char wordList[][WORD_LEN]){
    long index;
    int isAvailable = FALSE;

    while (!(isAvailable)){
        index = random() % WORDS;
        if (strlen(wordList[index]) > MINIMUM_LEN){
            isAvailable = TRUE;
        }
    }

    strcpy(word,wordList[index]);
}

void result(int gameTimes, int failureTimes, int correctTimes,char failureWords[][WORD_LEN]){
    double correctAnswerRate;
    correctAnswerRate = (double)correctTimes / (double)gameTimes;
    for(int i=0;i<failureTimes; i++){
        printf("%d回目に間違えた単語は%sです\n", i+1,failureWords[i]);
    }
    printf("正解数は%dです\n", correctTimes);
    printf("不正回数は%dです\n", failureTimes);
    printf("正解数の割合は%.1f％です\n", correctAnswerRate * 100);
}