#include<time.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#define chessB(piece)"\033[1;34m"#piece"\033[0m"//藍棋
#define chessR(piece)"\033[1;31m"#piece"\033[0m"//紅棋
#define coordin(piece)"\033[32m"#piece"\033[0m"//棋盤座標
#define coordinn(piece)"\033[33m"#piece"\033[0m"//駒台座標
#define line(piece)"\033[37m"#piece"\033[0m"//格線
#define cell(piece)"\033[8;30;40m"#piece"\033[0m"//空格
#define welcome(word)"\033[4;30;43m"#word"\033[0m"
#define min(i, j) (((i) < (j)) ? (i) : (j))//小的
#define max(i, j) (((i) > (j)) ? (i) : (j))//大的
#include <ev.h>
//定義變數
char input;
int xi,yi;//棋子
int xj,yj;//位置
int turn = -1;
bool chessSign = 1;
bool loadingsign = 1;
bool isStandard = 1;
bool GameOverSign = 1;
bool restart = 0;
bool skip = 0;
char tophandfile[1000];
char boardfile[1500];
char bothandfile[1000];
ev_timer time_watcher;
ev_io io_watcher;
time_t t1 = 0, t2 = 0;
char *board[10][10];
char *tophand[3][14];
char *bothand[3][14];
char *record = "我是大便";
int num1 = 0;

struct board_save{     //儲存  
    //把棋盤和駒台存起來
    char *saved_tophand[3][14];
    char *saved_board[10][10];
    char *saved_bothand[3][14];
    //輪轉紀錄
    int saved_red_or_blue;
    //左右指標
    struct board_save *left, *right;    
};
typedef struct board_save board_save;
board_save *list;
//函式
void boardbuilding();
void printboard();
int red_or_blue(int x,int y);
int tophandchek(int x,int y);
int bothandchek(int x,int y);
int blueMove(FILE *fptr);
int redMove(FILE *fptr);
int upgrade();
int captive(int x,int y);
int Turn_blueenemies_into_friends();
int Turn_redenemies_into_friends();
void rules_of_chesspieces();
void is_Gameover();
void LoadingLine();
board_save *Back(board_save *list, int turn);
board_save *Next(board_save *list, int turn);
void SavedBoard(board_save *list, int turn);
board_save *CreateSavedBoard(board_save *list);
char Turn_to_Symbol_bothand(int i,int j);
char Turn_to_Symbol_tophand(int i,int j);
char Turn_to_Symbol_board(int i,int j);
char *Turn_to_word_tophand(char tophandfile);
char *Turn_to_word_bothand(char bothandfile);
char *Turn_to_word_board(char boardfile);

static void timer_cb(EV_P_ ev_timer *w, int revents);
static void io_cb(EV_P_ ev_io *w, int revents);
void timemain();
//主函式
int main(int argc, char *argv[])
{
    FILE *fptr = fopen("new_game_file.txt","a+");
    //FILE *cfptr;
    FILE *cfptr = fopen("old_game_file2.txt","a+");
    list = (board_save*)malloc(sizeof(board_save));
    list->left = NULL;
    boardbuilding();//生成棋盤
    LoadingLine();
    system("clear");
    printf("%s\n",welcome(Welcome to shogi world~));
    SavedBoard(list, turn);
    printboard();//印出棋盤
    //開始下棋
    while(chessSign){
        printf("讀取棋譜請按r\n");
        printf("開始遊戲請按p\n");
        printf("離開請按l\n");
        scanf(" %c",&input);
        if(input == 'r'){
            int turnfile;
            if( NULL == cfptr ){
                printf("open failure");
                return 1;
            }
            for(int i =0;i<3;i++){
                for(int j=0;j<14;j++){
                    fscanf(cfptr," %c",&tophandfile[i*14+j]);
                    //tophand[i][j] = tophandfile[i*14+j];
                    tophand[i][j] = Turn_to_word_tophand(tophandfile[i*14+j]);
                }
            }
            for(int i =0 ;i<10;i++){
                for(int j=0;j<10;j++){
                    fscanf(cfptr," %c",&boardfile[i*10+j]);
                    //board[i][j] = boardfile[i*10+j];
                    board[i][j] = Turn_to_word_board(boardfile[i*10+j]);
                }
            }
            for(int i =0 ;i<3;i++){
                for(int j=0;j<14;j++){
                    fscanf(cfptr," %c",&bothandfile[i*14+j]);
                    //bothand[i][j] = bothandfile[i*14+j];
                    bothand[i][j] = Turn_to_word_bothand(bothandfile[i*14+j]);
                }
            }
            fscanf(cfptr,"%d",&turnfile);
            turn = turnfile;
            list = CreateSavedBoard(list);
            SavedBoard(list,turn);
            printboard();
            while(loadingsign){
                printf("移動下一手請按f\n");
                printf("回到上一手請按b\n");
                printf("開始遊戲請按p\n");
                printf("離開請按l\n");
                scanf(" %c",&input);
                if(input == 'l'){
                    loadingsign = 0;
                }
                else if(input == 'p'){
                    loadingsign = 0;
                }
                else if(input == 'b'){
                    list = Back(list,turn);
                    printboard();
                }
                else if(input == 'f'){
                    int turnfile;
                    if( NULL == cfptr ){
                        printf("open failure");
                        return 1;
                    }
                    for(int i =0 ;i<3;i++){
                        for(int j=0;j<14;j++){
                            fscanf(cfptr," %c",&tophandfile[i*14+j]);
                            //tophand[i][j] = tophandfile[i*14+j];
                            tophand[i][j] = Turn_to_word_tophand(tophandfile[i*14+j]);
                        }
                    }
                    for(int i =0 ;i<10;i++){
                        for(int j=0;j<10;j++){
                            fscanf(cfptr," %c",&boardfile[i*10+j]);
                            //board[i][j] = boardfile[i*10+j];
                            board[i][j] = Turn_to_word_board(boardfile[i*10+j]);
                        }
                    }
                    for(int i =0 ;i<3;i++){
                        for(int j=0;j<14;j++){
                            fscanf(cfptr," %c",&bothandfile[i*14+j]);
                            //bothand[i][j] = bothandfile[i*14+j];
                            bothand[i][j] = Turn_to_word_bothand(bothandfile[i*14+j]);
                        }
                    }
                    fscanf(cfptr,"%d",&turnfile);
                    turn = turnfile;
                    list = CreateSavedBoard(list);
                    SavedBoard(list,turn);
                    //list = Next(list,turn);
                    printboard();
                }
            }
        }
        if(input == 'p'){
            GameOverSign = 1;
            while(GameOverSign){
                num1 = 1;
                isStandard = 1;
                turn *= (-1);//換邊
                switch(turn){
                    case 1:
                        redMove(fptr);
                        turn = (restart) ? (turn*-1) : turn;
                        break;
                    case -1:
                        blueMove(fptr);
                        turn = (restart) ? (turn*-1) : turn;
                        break;
                }
                is_Gameover();
            }
            printf("━━━━━┒\n");
            printf("┓┏┓┏┓┃\n");
            printf("┛┗┛┗┛┃gmae over!\n");
            printf("┓┏┓┏┓┃＼😭／\n");
            printf("┛┗┛┗┛┃　/\n");
            printf("┓┏┓┏┓┃ノ)\n");
            printf("┛┗┛┗┛┃\n");
            printf("┓┏┓┏┓┃\n");
            printf("┛┗┛┗┛┃\n");
            printf("┓┏┓┏┓┃\n");
            printf("┛┗┛┗┛┃\n");
            printf("┓┏┓┏┓┃\n");
            printf("┃┃┃┃┃┃\n");
            printf("┻┻┻┻┻┻\n");
            num1 = 0;
        }
        if(input == 'l'){
            chessSign = 0;
        }
    }
    fclose(fptr);
}
//生成棋盤
void boardbuilding()
{   //hand
    for(int i=0;i<3;i++){
        for(int j=0;j<14;j++){
            tophand[i][j] = cell(空);
            bothand[i][j] = cell(空);
        }
    }
    tophand[1][0] = tophand[2][11] = tophand[1][11] = tophand[2][0] = line(|);
    tophand[1][12] = coordinn(一);
    tophand[2][12] = coordinn(二);
    tophand[2][13] = line(#tophand駒台);
    tophand[0][10] = coordinn(十);
    bothand[0][0] = bothand[1][11] = bothand[0][11] = bothand[1][0] = line(|);
    bothand[0][12] = coordinn(一);
    bothand[1][12] = coordinn(二);
    bothand[1][13] = line(#bottomhand駒台);
    bothand[2][10] = coordinn(十);
    //chessboard
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            board[i][j] = cell(空);
        }
    }
    board[0][0] = coordin(九);
    board[0][1] = coordin(八);
    board[0][2] = coordin(七);
    board[0][3] = coordin(六);
    board[0][4] = coordin(五);
    board[0][5] = coordin(四);
    board[0][6] = coordin(三);
    board[0][7] = coordin(二);
    board[0][8] = coordin(一);
    board[1][9] = coordin(一); tophand[0][1] = bothand[2][1] = coordinn(一);
    board[2][9] = coordin(二); tophand[0][2] = bothand[2][2] = coordinn(二);
    board[3][9] = coordin(三); tophand[0][3] = bothand[2][3] = coordinn(三);
    board[4][9] = coordin(四); tophand[0][4] = bothand[2][4] = coordinn(四);
    board[5][9] = coordin(五); tophand[0][5] = bothand[2][5] = coordinn(五);
    board[6][9] = coordin(六); tophand[0][6] = bothand[2][6] = coordinn(六);
    board[7][9] = coordin(七); tophand[0][7] = bothand[2][7] = coordinn(七);
    board[8][9] = coordin(八); tophand[0][8] = bothand[2][8] = coordinn(八);
    board[9][9] = coordin(九); tophand[0][9] = bothand[2][9] = coordinn(九);
    board[1][0] = board[1][8] = chessR(香);
    board[9][0] = board[9][8] = chessB(香);
    board[1][1] = board[1][7] = chessR(桂);
    board[9][1] = board[9][7] = chessB(桂);
    board[1][2] = board[1][6] = chessR(銀);
    board[9][2] = board[9][6] = chessB(銀);
    board[1][3] = board[1][5] = chessR(金);
    board[9][3] = board[9][5] = chessB(金);
    board[2][1] = chessR(飛);
    board[8][7] = chessB(飛);
    board[8][1] = chessB(角);
    board[2][7] = chessR(角);
    board[1][4] = chessR(王);
    board[9][4] = chessB(玉);
    for(int i=0;i<9;i++){
        board[3][i] = chessR(步);
    }
    for(int i=0;i<9;i++){
        board[7][i] = chessB(步);
    }
}
//印出棋盤
void printboard()
{
    //顯示
    //tophand
    printf("%s",cell(|));
    for(int i=1;i<11;i++){
        printf("%s",tophand[0][i]);
        printf("%s",cell(|));
    }
    printf("\n");
    for(int i=0;i<16;i++){
        printf("%s",line(——));
    }
    printf("\n");
    for(int i=1;i<3;i++){
        printf("%s",tophand[i][0]);
        for(int j=1;j<14;j++){
            printf("%s",tophand[i][j]);
            printf("%s",cell(|));
        }
        printf("\n");
    }
    for(int i=0;i<16;i++){
        printf("%s",line(——));
    }
    printf("\n");
    //chessboard
    for(int i=0;i<10;i++){
        printf("%s",cell(|));
        printf("%s",board[0][i]);
    }
    printf("\n");
    for(int i=1;i<10;i++){
        for(int i=0;i<14;i++){
            printf("%s",line(——));
        }
        printf("\n");
        for(int j=0;j<10;j++){
            printf("%s",line(|));
            printf("%s",board[i][j]);
        }
        printf("\n");
    }
    //bottomhand
    for(int i=0;i<16;i++){
        printf("%s",line(——));
    }
    printf("\n");
    for(int i=0;i<2;i++){
        printf("%s",bothand[i][0]);
        for(int j=1;j<14;j++){
            printf("%s",bothand[i][j]);
            printf("%s",cell(|));
        }
        printf("\n");
    }
    for(int i=0;i<16;i++){
        printf("%s",line(——));
    }
    printf("\n");
    printf("%s",cell(|));
    for(int i=1;i<11;i++){
        printf("%s",bothand[2][i]);
        printf("%s",cell(|));
    }
    printf("\n");
    if(num1 == 1){
        puts(record);
    }
}
//轉成字母
char Turn_to_Symbol_bothand(int i,int j){
    if(bothand[i][j] == chessR(杏)){
                return('o');
            }else if(bothand[i][j] == chessR(圭)){
                return('h');
            }else if(bothand[i][j] == chessR(全)){
                return('w');
            }else if(bothand[i][j] == chessR(と)){
                return('m');
            }else if(bothand[i][j] == chessR(龍)){
                return('f');
            }else if(bothand[i][j] == chessR(馬)){
                return('t');
            }else if(bothand[i][j] == chessB(杏)){
                return('O');
            }else if(bothand[i][j] == chessB(圭)){
                return('H');
            }else if(bothand[i][j] == chessB(全)){
                return('W');
            }else if(bothand[i][j] == chessB(と)){
                return('M');
            }else if(bothand[i][j] == chessB(龍)){
                return('F');
            }else if(bothand[i][j] == chessB(馬)){
                return('T');
            }else if(bothand[i][j] == chessR(香)){
                return('l');
            }else if(bothand[i][j] == chessR(桂)){
                return('n');
            }else if(bothand[i][j] == chessR(銀)){
                return('s');
            }else if(bothand[i][j] == chessR(步)){
                return('p');
            }else if(bothand[i][j] == chessR(飛)){
                return('r');
            }else if(bothand[i][j] == chessR(角)){
                return('b');
            }else if(bothand[i][j] == chessB(香)){
                return('L');
            }else if(bothand[i][j] == chessB(桂)){
                return('N');
            }else if(bothand[i][j] == chessB(銀)){
                return('S');
            }else if(bothand[i][j] == chessB(步)){
                return('P');
            }else if(bothand[i][j] == chessB(飛)){
                return('R');
            }else if(bothand[i][j] == chessB(角)){
                return('B');
            }else if(bothand[i][j] == chessR(王)){
                return('K');
            }else if(bothand[i][j] == chessB(玉)){
                return('k');
            }else if(bothand[i][j] == cell(空)){
                return('A');
            }else if(bothand[i][j] == chessB(金)){
                return('G');
            }else if(bothand[i][j] == chessR(金)){
                return('g');
            }
            else if(bothand[i][j] == coordinn(一)){
                return('1');
            }
            else if(bothand[i][j] == coordinn(二)){
                return('2');
            }
            else if(bothand[i][j] == coordinn(三)){
                return('3');
            }
            else if(bothand[i][j] == coordinn(四)){
                return('4');
            }
            else if(bothand[i][j] == coordinn(五)){
                return('5');
            }
            else if(bothand[i][j] == coordinn(六)){
                return('6');
            }
            else if(bothand[i][j] == coordinn(七)){
                return('7');
            }
            else if(bothand[i][j] == coordinn(八)){
                return('8');
            }
            else if(bothand[i][j] == coordinn(九)){
                return('9');
            }
            else if(bothand[i][j] == coordinn(十)){
                return('c');
            }
            else if(bothand[i][j] == line(#bottomhand駒台)){
                return('C');
            }
            else if(bothand[i][j] == line(|)){
                return('e');
            }
}
char Turn_to_Symbol_board(int i,int j){
    if(board[i][j] == chessR(杏)){
                return('o');
            }else if(board[i][j] == chessR(圭)){
                return('h');
            }else if(board[i][j] == chessR(全)){
                return('w');
            }else if(board[i][j] == chessR(と)){
                return('m');
            }else if(board[i][j] == chessR(龍)){
                return('f');
            }else if(board[i][j] == chessR(馬)){
                return('t');
            }else if(board[i][j] == chessB(杏)){
                return('O');
            }else if(board[i][j] == chessB(圭)){
                return('H');
            }else if(board[i][j] == chessB(全)){
                return('W');
            }else if(board[i][j] == chessB(と)){
                return('M');
            }else if(board[i][j] == chessB(龍)){
                return('F');
            }else if(board[i][j] == chessB(馬)){
                return('T');
            }else if(board[i][j] == chessR(香)){
                return('l');
            }else if(board[i][j] == chessR(桂)){
                return('n');
            }else if(board[i][j] == chessR(銀)){
                return('s');
            }else if(board[i][j] == chessR(步)){
                return('p');
            }else if(board[i][j] == chessR(飛)){
                return('r');
            }else if(board[i][j] == chessR(角)){
                return('b');
            }else if(board[i][j] == chessB(香)){
                return('L');
            }else if(board[i][j] == chessB(桂)){
                return('N');
            }else if(board[i][j] == chessB(銀)){
                return('S');
            }else if(board[i][j] == chessB(步)){
                return('P');
            }else if(board[i][j] == chessB(飛)){
                return('R');
            }else if(board[i][j] == chessB(角)){
                return('B');
            }else if(board[i][j] == chessR(王)){
                return('K');
            }else if(board[i][j] == chessB(玉)){
                return('k');
            }else if(board[i][j] == cell(空)){
                return('A');
            }else if(board[i][j] == chessB(金)){
                return('G');
            }else if(board[i][j] == chessR(金)){
                return('g');
            }
            else if(board[i][j] == coordin(一)){
                return('i');
            }
            else if(board[i][j] == coordin(二)){
                return('I');
            }
            else if(board[i][j] == coordin(三)){
                return('j');
            }
            else if(board[i][j] == coordin(四)){
                return('J');
            }
            else if(board[i][j] == coordin(五)){
                return('q');
            }
            else if(board[i][j] == coordin(六)){
                return('Q');
            }
            else if(board[i][j] == coordin(七)){
                return('u');
            }
            else if(board[i][j] == coordin(八)){
                return('U');
            }
            else if(board[i][j] == coordin(九)){
                return('x');
            }
}
char Turn_to_Symbol_tophand(int i,int j){
    if(tophand[i][j] == chessR(杏)){
        return('o');
    }else if(tophand[i][j] == chessR(圭)){
        return('h');
    }else if(tophand[i][j] == chessR(全)){
        return('w');
    }else if(tophand[i][j] == chessR(と)){
        return('m');
    }else if(tophand[i][j] == chessR(龍)){
        return('f');
    }else if(tophand[i][j] == chessR(馬)){
        return('t');
    }else if(tophand[i][j] == chessB(杏)){
        return('O');
    }else if(tophand[i][j] == chessB(圭)){
        return('H');
    }else if(tophand[i][j] == chessB(全)){
        return('W');
    }else if(tophand[i][j] == chessB(と)){
        return('M');
    }else if(tophand[i][j] == chessB(龍)){
        return('F');
    }else if(tophand[i][j] == chessB(馬)){
        return('T');
    }else if(tophand[i][j] == chessR(香)){
        return('l');
    }else if(tophand[i][j] == chessR(桂)){
        return('n');
    }else if(tophand[i][j] == chessR(銀)){
        return('s');
    }else if(tophand[i][j] == chessR(步)){
        return('p');
    }else if(tophand[i][j] == chessR(飛)){
        return('r');
    }else if(tophand[i][j] == chessR(角)){
        return('b');
    }else if(tophand[i][j] == chessB(香)){
        return('L');
    }else if(tophand[i][j] == chessB(桂)){
        return('N');
    }else if(tophand[i][j] == chessB(銀)){
        return('S');
    }else if(tophand[i][j] == chessB(步)){
        return('P');
    }else if(tophand[i][j] == chessB(飛)){
        return('R');
    }else if(tophand[i][j] == chessB(角)){
        return('B');
    }else if(tophand[i][j] == chessR(王)){
        return('K');
    }else if(tophand[i][j] == chessB(玉)){
        return('k');
    }else if(tophand[i][j] == cell(空)){
        return('A');
    }else if(tophand[i][j] == chessB(金)){
        return('G');
    }else if(tophand[i][j] == chessR(金)){
        return('g');
    } 
    else if(tophand[i][j] == coordinn(一)){
                return('1');
            }
            else if(tophand[i][j] == coordinn(二)){
                return('2');
            }
            else if(tophand[i][j] == coordinn(三)){
                return('3');
            }
            else if(tophand[i][j] == coordinn(四)){
                return('4');
            }
            else if(tophand[i][j] == coordinn(五)){
                return('5');
            }
            else if(tophand[i][j] == coordinn(六)){
                return('6');
            }
            else if(tophand[i][j] == coordinn(七)){
                return('7');
            }
            else if(tophand[i][j] == coordinn(八)){
                return('8');
            }
            else if(tophand[i][j] == coordinn(九)){
                return('9');
            }
            else if(tophand[i][j] == coordinn(十)){
                return('c');
            }
            else if(tophand[i][j] == line(#tophand駒台)){
                return('E');
            }
            else if(tophand[i][j] == line(|)){
                return('e');
            }
}
//字母轉字
char *Turn_to_word_tophand(char tophandfile){
    if(tophandfile == 'o'){
        return(chessR(杏));
    }else if(tophandfile == 'h'){
        return(chessR(圭));
    }else if(tophandfile == 'w'){
        return(chessR(全));
    }else if(tophandfile == 'm'){
        return(chessR(と));
    }else if(tophandfile == 'f'){
      return(chessR(龍));
    }else if(tophandfile == 't'){
        return(chessR(馬));
    }else if(tophandfile == 'O'){
        return(chessB(杏));
    }else if(tophandfile == 'H'){
        return(chessB(圭));
    }else if(tophandfile == 'W'){
        return(chessB(全));
    }else if(tophandfile == 'M'){
        return(chessB(と));
    }else if(tophandfile == 'F'){
        return(chessB(龍));
    }else if(tophandfile == 'T'){
        return(chessB(馬));
    }else if(tophandfile == 'l'){
        return(chessR(香));
    }else if(tophandfile == 'n'){
        return(chessR(桂));
    }else if(tophandfile == 's'){
        return(chessR(銀));
    }else if(tophandfile == 'p'){
        return(chessR(步));
    }else if(tophandfile == 'r'){
        return(chessR(飛));
    }else if(tophandfile== 'b'){
        return(chessR(角));
    }else if(tophandfile == 'L'){
        return(chessB(香));
    }else if(tophandfile == 'N'){
        return(chessB(桂));
    }else if(tophandfile == 'S'){
        return(chessB(銀));
    }else if(tophandfile == 'P'){
        return(chessB(步));
    }else if(tophandfile == 'R'){
        return(chessB(飛));
    }else if(tophandfile == 'B'){
        return(chessB(角));
    }else if(tophandfile == 'K'){
        return(chessR(王));
    }else if(tophandfile == 'k'){
        return(chessB(玉));
    }else if(tophandfile == 'A'){
        return(cell(空));
    }else if(tophandfile == 'G'){
        return(chessB(金));
    }else if(tophandfile == 'g'){
        return(chessR(金));
    }else if(tophandfile == '1'){
        return(coordinn(一));
    }else if(tophandfile == '2'){
        return(coordinn(二));
    }else if(tophandfile == '3'){
        return(coordinn(三));
    }else if(tophandfile == '4'){
        return(coordinn(四));
    }else if(tophandfile == '5'){
        return(coordinn(五));
    }else if(tophandfile == '6'){
        return(coordinn(六));
    }else if(tophandfile == '7'){
        return(coordinn(七));
    }else if(tophandfile == '8'){
        return(coordinn(八));
    }else if(tophandfile == '9'){
        return(coordinn(九));
    }else if(tophandfile == 'c'){
        return(coordinn(十));
    }else if(tophandfile == 'E'){
        return(line(#tophand駒台));
    }else if(tophandfile == 'e'){
        return(line(|));
    }else{
        return(chessB(媽));
    }
}
char *Turn_to_word_bothand(char bothandfile){
    if(bothandfile == 'o'){
        return(chessR(杏));
    }else if(bothandfile == 'h'){
        return(chessR(圭));
    }else if(bothandfile == 'w'){
        return(chessR(全));
    }else if(bothandfile == 'm'){
        return(chessR(と));
    }else if(bothandfile == 'f'){
      return(chessR(龍));
    }else if(bothandfile == 't'){
        return(chessR(馬));
    }else if(bothandfile == 'O'){
        return(chessB(杏));
    }else if(bothandfile == 'H'){
        return(chessB(圭));
    }else if(bothandfile == 'W'){
        return(chessB(全));
    }else if(bothandfile == 'M'){
        return(chessB(と));
    }else if(bothandfile == 'F'){
        return(chessB(龍));
    }else if(bothandfile == 'T'){
        return(chessB(馬));
    }else if(bothandfile == 'l'){
        return(chessR(香));
    }else if(bothandfile == 'n'){
        return(chessR(桂));
    }else if(bothandfile == 's'){
        return(chessR(銀));
    }else if(bothandfile == 'p'){
        return(chessR(步));
    }else if(bothandfile == 'r'){
        return(chessR(飛));
    }else if(bothandfile == 'b'){
        return(chessR(角));
    }else if(bothandfile == 'L'){
        return(chessB(香));
    }else if(bothandfile == 'N'){
        return(chessB(桂));
    }else if(bothandfile == 'S'){
        return(chessB(銀));
    }else if(bothandfile == 'P'){
        return(chessB(步));
    }else if(bothandfile == 'R'){
        return(chessB(飛));
    }else if(bothandfile == 'B'){
        return(chessB(角));
    }else if(bothandfile == 'K'){
        return(chessR(王));
    }else if(bothandfile == 'k'){
        return(chessB(玉));
    }else if(bothandfile == 'A'){
        return(cell(空));
    }else if(bothandfile == 'G'){
        return(chessB(金));
    }else if(bothandfile == 'g'){
        return(chessR(金));
    }else if(bothandfile == '1'){
        return(coordinn(一));
    }else if(bothandfile == '2'){
        return(coordinn(二));
    }else if(bothandfile == '3'){
        return(coordinn(三));
    }else if(bothandfile == '4'){
        return(coordinn(四));
    }else if(bothandfile == '5'){
        return(coordinn(五));
    }else if(bothandfile == '6'){
        return(coordinn(六));
    }else if(bothandfile == '7'){
        return(coordinn(七));
    }else if(bothandfile == '8'){
        return(coordinn(八));
    }else if(bothandfile == '9'){
        return(coordinn(九));
    }else if(bothandfile == 'c'){
        return(coordinn(十));
    }else if(bothandfile == 'C'){
        return(line(#bottomhand駒台));
    }else if(bothandfile == 'e'){
        return(line(|));
    }else{
        return(chessB(幹));
    }
}
char *Turn_to_word_board(char boardfile){
    if(boardfile == 'o'){
        return(chessR(杏));
    }else if(boardfile == 'h'){
        return(chessR(圭));
    }else if(boardfile == 'w'){
        return(chessR(全));
    }else if(boardfile == 'm'){
        return(chessR(と));
    }else if(boardfile == 'f'){
      return(chessR(龍));
    }else if(boardfile == 't'){
        return(chessR(馬));
    }else if(boardfile == 'O'){
        return(chessB(杏));
    }else if(boardfile == 'H'){
        return(chessB(圭));
    }else if(boardfile == 'W'){
        return(chessB(全));
    }else if(boardfile == 'M'){
        return(chessB(と));
    }else if(boardfile == 'F'){
        return(chessB(龍));
    }else if(boardfile == 'T'){
        return(chessB(馬));
    }else if(boardfile == 'l'){
        return(chessR(香));
    }else if(boardfile == 'n'){
        return(chessR(桂));
    }else if(boardfile == 's'){
        return(chessR(銀));
    }else if(boardfile == 'p'){
        return(chessR(步));
    }else if(boardfile == 'r'){
        return(chessR(飛));
    }else if(boardfile == 'b'){
        return(chessR(角));
    }else if(boardfile == 'L'){
        return(chessB(香));
    }else if(boardfile == 'N'){
        return(chessB(桂));
    }else if(boardfile == 'S'){
        return(chessB(銀));
    }else if(boardfile == 'P'){
        return(chessB(步));
    }else if(boardfile == 'R'){
        return(chessB(飛));
    }else if(boardfile == 'B'){
        return(chessB(角));
    }else if(boardfile == 'K'){
        return(chessR(王));
    }else if(boardfile == 'k'){
        return(chessB(玉));
    }else if(boardfile == 'A'){
        return(cell(空));
    }else if(boardfile == 'G'){
        return(chessB(金));
    }else if(boardfile == 'g'){
        return(chessR(金));
    }else if(boardfile == 'i'){
        return(coordin(一));
    }else if(boardfile == 'I'){
        return(coordin(二));
    }else if(boardfile == 'j'){
        return(coordin(三));
    }else if(boardfile == 'J'){
        return(coordin(四));
    }else if(boardfile == 'q'){
        return(coordin(五));
    }else if(boardfile == 'Q'){
        return(coordin(六));
    }else if(boardfile == 'u'){
        return(coordin(七));
    }else if(boardfile == 'U'){
        return(coordin(八));
    }else if(boardfile == 'x'){
        return(coordin(九));
    }else{
        return(chessB(哭));
    }
}
//跑條
void LoadingLine(){
    int i, j, k;
    for(i = 0;i <1000;i++){
        system("clear");
        printf("|");
        for(j = 0;j < i;j+=10) {
            printf("█");
        }
        printf("|Loadng...\n");
    }
}
//判斷藍棋或紅棋或座標
int red_or_blue(int x,int y)
{
    if(board[x][y]==chessR(香)||board[x][y]==chessR(桂)||board[x][y]==chessR(銀)||board[x][y]==chessR(金)||board[x][y]==chessR(王)||board[x][y]==chessR(飛)||board[x][y]==chessR(角)||board[x][y]==chessR(步)||board[x][y]==chessR(杏)||board[x][y]==chessR(圭)||board[x][y]==chessR(と)||board[x][y]==chessR(全)||board[x][y]==chessR(龍)||board[x][y]==chessR(馬)){
        return 1;
    }
    else if(board[x][y]==chessB(香)||board[x][y]==chessB(桂)||board[x][y]==chessB(銀)||board[x][y]==chessB(金)||board[x][y]==chessB(王)||board[x][y]==chessB(飛)||board[x][y]==chessB(角)||board[x][y]==chessB(步)||board[x][y]==chessB(杏)||board[x][y]==chessB(圭)||board[x][y]==chessB(と)||board[x][y]==chessB(全)||board[x][y]==chessB(龍)||board[x][y]==chessB(馬)){
        return -1;
    }
    else if(board[x][y]==coordin(九)||board[x][y]==coordin(八)||board[x][y]==coordin(七)||board[x][y]==coordin(六)||board[x][y]==coordin(五)||board[x][y]==coordin(四)||board[x][y]==coordin(三)||board[x][y]==coordin(二)||board[x][y]==coordin(一)||(x==0 && y==9)){
        return 2;
    }
    else 
        return 0;
}
//判斷bothand
int bothandchek(int x,int y){
    if(bothand[x][y]==chessR(香)||bothand[x][y]==chessR(桂)||bothand[x][y]==chessR(銀)||bothand[x][y]==chessR(金)||bothand[x][y]==chessR(王)||bothand[x][y]==chessR(飛)||bothand[x][y]==chessR(角)||bothand[x][y]==chessR(步)||bothand[x][y]==chessR(杏)||bothand[x][y]==chessR(全)||bothand[x][y]==chessR(と)||bothand[x][y]==chessR(圭)||bothand[x][y]==chessR(龍)||bothand[x][y]==chessR(馬)){
        return 4;
    }
    else
        return 0;
}
//判斷tophand
int tophandchek(int x,int y){
    if(tophand[x][y]==chessB(香)||tophand[x][y]==chessB(桂)||tophand[x][y]==chessB(銀)||tophand[x][y]==chessB(金)||tophand[x][y]==chessB(飛)||tophand[x][y]==chessB(角)||tophand[x][y]==chessB(步)||tophand[x][y]==chessB(全)||tophand[x][y]==chessB(杏)||tophand[x][y]==chessB(圭)||tophand[x][y]==chessB(と)||tophand[x][y]==chessB(全)||tophand[x][y]==chessB(馬)||tophand[x][y]==chessB(龍)){
        return 3;
    }
    else
        return 0;
}
//紅棋移動
int redMove(FILE *fptr)
{   
    char yn;
    bool start = 1;
    char leave;
    char arr1[]={"是否離開,離開請按l,不離開請按n,儲存請按s:"};
    record =arr1;
    timemain();
    scanf(" %c",&leave);
    if(leave == 'l'){
        GameOverSign = 0;
        return 0;
    }
    if(restart){
        printf("違反遊戲規則請重新輸入\n");
        restart = 0;
    }
    if(list->left != NULL ){    //回到上一手
        //printf("是否回到上一手[y/n]: ");
        char arr2[]={"是否回到上一手[y/n]: "};
        record =arr2;
        timemain();
        scanf(" %c",&yn);
        if(yn == 'y'){
            list = Back(list,turn);
            printf("%s\n",coordin(=================================================));
            printboard();
            return 0;
        }
        if(yn != 'n' && yn != 'y'){
            restart = 1;
            return 0;
        }
    }
    Turn_blueenemies_into_friends();
    if(skip){
        skip = 0;
        return 0;
    }
    char arr3[]={"[紅]請輸入棋子座標的段(行): "};
    record =arr3;
    timemain();
    //printf("[紅]請輸入棋子座標的段(行): ");
    scanf("%d",&xi);
    char arr4[]={"[紅]請輸入棋子座標的筋(列): "};
    record =arr4;
    timemain();
    //printf("[紅]請輸入棋子座標的筋(列): ");
    scanf("%d",&yi);
    if(xi>9 || yi>9){
        restart = 1;
        return 0;
    }
    yi=(yi-9)*(-1);
    if(red_or_blue(xi,yi)==-1||red_or_blue(xi,yi)==2||board[xi][yi]==cell(空)){
        restart = 1;
        return 0;
    }
    char arr5[]={"[紅]請輸入想要放置的座標的段(行): "};
    record =arr5;
    timemain();
    //printf("[紅]請輸入想要放置的座標的段(行): ");
    scanf("%d",&xj);
    char arr6[]={"[紅]請輸入想要放置的座標的筋(列): "};
    record =arr6;
    timemain();
    //printf("[紅]請輸入想要放置的座標的筋(列): ");
    scanf("%d",&yj);
    rules_of_chesspieces();
    list = CreateSavedBoard(list);
    if( NULL == fptr ){
        printf("open failure\n");
        return 1;
    }
    for(int i =0 ;i<3;i++){
        for(int j=0;j<14;j++){
            //Turn_to_Symbol_tophand(i,j)
            fprintf(fptr,"%c\n",Turn_to_Symbol_tophand(i,j));
        }
    }
    for(int i =0 ;i<10;i++){
        for(int j=0;j<10;j++){
            //Turn_to_Symbol_board(i,j)
            fprintf(fptr,"%c\n",Turn_to_Symbol_board(i,j));
        }
    }
    for(int i =0 ;i<3;i++){
        for(int j=0;j<14;j++){
            //Turn_to_Symbol_bothand(i,j)
            fprintf(fptr,"%c\n",Turn_to_Symbol_bothand(i,j));
        }
    }
    fprintf(fptr,"%d\n",turn);
    SavedBoard(list,turn);
    printf("%s\n",coordin(=================================================));
    printboard();
}
//藍棋移動
int blueMove(FILE *fptr)
{
    char yn;
    char leave;
    char arr7[]={"是否離開,離開請按l,不離開請按n,儲存請按s:"};
    record =arr7;
    timemain();
    //printf("是否離開,離開請按l,不離開請按n,儲存請按s:");
    scanf(" %c",&leave);
    if(leave == 'l'){
        GameOverSign = 0;
        return 0;
    }
    if(restart){
        printf("違反遊戲規則請重新輸入\n");
        restart=0;
    }
    if(list->left != NULL ){    //回到上一手
        char arr8[]={"是否回到上一手[y/n]: "};
        record =arr8;
        timemain();
        //printf("是否回到上一手[y/n]: ");
        scanf(" %c",&yn);
        if(yn == 'y'){
            list = Back(list,turn);
            printf("%s\n",coordin(=================================================));
            printboard();
            return 0;
        }
        if(yn != 'n' && yn != 'y'){
            restart = 1;
            return 0;
        }
    }
    Turn_redenemies_into_friends();
    if(skip){
        skip = 0;
        return 0;
    }
    char arr9[]={"[藍]請輸入棋子座標的段(行): "};
    record =arr9;
    timemain();
    //printf("[藍]請輸入棋子座標的段(行): ");
    scanf("%d",&xi);
    char arr10[]={"[藍]請輸入棋子座標的筋(列): "};
    record =arr10;
    timemain();
    //printf("[藍]請輸入棋子座標的筋(列): ");
    scanf("%d",&yi);
    if(xi>9 || yi>9){
        restart = 1;
        return 0;
    }
    yi=(yi-9)*(-1);
    if(red_or_blue(xi,yi)==1||red_or_blue(xi,yi)==2||board[xi][yi]==cell(空)){
        restart = 1;
        return 0;
    }
    char arr11[]={"[藍]請輸入想要放置的座標的段(行): "};
    record =arr11;
    timemain();
    //printf("[藍]請輸入想要放置的座標的段(行): ");
    scanf("%d",&xj);
    char arr12[]={"[藍]請輸入想要放置的座標的筋(列): "};
    record =arr12;
    timemain();
    //printf("[藍]請輸入想要放置的座標的筋(列): ");
    scanf("%d",&yj);
    rules_of_chesspieces();
    list = CreateSavedBoard(list);
    if( NULL == fptr ){
        printf("open failure\n");
        return 1;
    }
    for(int i =0 ;i<3;i++){
        for(int j=0;j<14;j++){
            //Turn_to_Symbol_tophand(i,j)
            fprintf(fptr,"%c\n",Turn_to_Symbol_tophand(i,j));
        }
    }
    for(int i =0 ;i<10;i++){
        for(int j=0;j<10;j++){
            //Turn_to_Symbol_board(i,j)
            fprintf(fptr,"%c\n",Turn_to_Symbol_board(i,j));
        }
    }
    for(int i =0 ;i<3;i++){
        for(int j=0;j<14;j++){
            //Turn_to_Symbol_bothand(i,j)
            fprintf(fptr,"%c\n",Turn_to_Symbol_bothand(i,j));
        }
    }
    fprintf(fptr,"%d \n",turn);
    SavedBoard(list,turn);
    printf("%s\n",coordin(=================================================));
    printboard();
}
//判斷遊戲結束
void is_Gameover()
{
    bool sign_R=0;
    bool sign_B=0;
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            if(board[i][j] == chessR(王)){
                sign_R = 1;
            }
            else if(board[i][j] == chessB(玉)){
                sign_B = 1;
            }
        }
    }
    if (sign_R == 0){
        GameOverSign =0;
        printf("藍方勝利");
    }
    else if(sign_B == 0){
        GameOverSign =0;
        printf("紅方勝利");
    }
}
//棋子規則
void rules_of_chesspieces()
{
    yj=(yj-9)*(-1);
    //chessR香車
    if(board[xi][yi] == chessR(香)){
        if(xi>xj){
            isStandard = 0;
        }
        if(yi==yj){
            for(int i=xi+1;i<xj;i++){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if((yi==yj) && isStandard && (red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(香);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessB香車
    else if(board[xi][yi] == chessB(香)){
        if(xi<xj){
            isStandard = 0;
        }
        if(yi==yj){
            for(int i=xi-1;i>xj;i--){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if((yi==yj) && isStandard && (red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(香);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessR桂馬
    else if(board[xi][yi] == chessR(桂)){
        if((red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2) && (xj == xi+2 && yj == yi-1) || (xj == xi+2 && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(桂);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessB桂馬
    else if(board[xi][yi] == chessB(桂)){
        if((red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2) && (xj == xi-2 && yj == yi-1) || (xj == xi-2 && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(桂);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessR步兵
    else if(board[xi][yi] == chessR(步)){
        if((xj != xi+1 || yj != yi)){
            isStandard = 0;
        }
        if((yi == yj) && isStandard && red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(步);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessB步兵
    else if(board[xi][yi] == chessB(步)){
         if((xj != xi-1 || yj != yi)){
            isStandard = 0;
        }
        if((yi == yj) && isStandard && red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(步);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessR銀將
    else if(board[xi][yi] == chessR(銀)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi+1) || (xj == xi-1 && yj == yi-1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(銀);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessB銀將
    else if(board[xi][yi] == chessB(銀)){
        if(red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2 && (xj == xi-1 && yj == yi) || (xj == xi-1 && yj == yi+1) || (xj == xi-1 && yj == yi-1) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(銀);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessR金將
    else if(board[xi][yi] == chessR(金)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(金);
        }
        else{
            restart = 1;
        }
    }
    //chessB金將
    else if(board[xi][yi] == chessB(金)){
        if(red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2 && (xj == xi-1 && yj == yi) || (xj == xi-1 && yj == yi+1) || (xj == xi-1 && yj == yi-1) || (xj == xi+1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(金);
        }
        else{
            restart = 1;
        }
    }
    //chessR飛車
    else if(board[xi][yi] == chessR(飛)){
        if(yi==yj){
            for(int i=xi+1;i<xj;i++){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
            for(int i=xi-1;i>xj;i--){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if(xi==xj){
            for(int i=yi+1;i<yj;i++){
                if(board[xi][i]!=cell(空)){
                    isStandard = 0;
                }
            }
            for(int i=xi-1;i>xj;i--){
                if(board[xi][i]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if((xi==xj || yi==yj) && isStandard && (red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(飛);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessB飛車
    else if(board[xi][yi] == chessB(飛)){
        if(yi==yj){
            for(int i=xi+1;i<xj;i++){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
            for(int i=xi-1;i>xj;i--){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if(xi==xj){
            for(int i=yi+1;i<yj;i++){
                if(board[xi][i]!=cell(空)){
                    isStandard = 0;
                }
            }
            for(int i=xi-1;i>xj;i--){
                if(board[xi][i]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if((xi==xj || yi==yj) && isStandard && (red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(飛);
            upgrade();
        }
        else{
            restart = 1;
        }
    }
    //chessR角行
    else if(board[xi][yi] == chessR(角)){
        for(int i=xi+1,j=yi+1;i<=xj,j<=yj;i++,j++){
            if(xj==i && yj==j){
                for(int i=xi+1,j=yi+1;i<xj,j<yj;i++,j++){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi+1,j=yi-1;i<=xj,j>=yj;i++,j--){
            if(xj==i && yj==j){
                for(int i=xi+1,j=yi-1;i<xj,j>yj;i++,j--){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi-1,j=yi+1;i>=xj,j<=yj;i--,j++){
            if(xj==i && yj==j){
                for(int i=xi-1,j=yi+1;i>xj,j<yj;i--,j++){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi-1,j=yi-1;i>=xj,j>=yj;i--,j--){
            if(xj==i && yj==j){
                for(int i=xi-1,j=yi-1;i>xj,j>yj;i--,j--){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        if(max(xi,xj)-min(xi,xj)==max(yi,yj)-min(yi,yj) && isStandard && red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(角);
            upgrade();
        }
        else{
            restart =1;
        }
    }
    //chessB角行
    else if(board[xi][yi] == chessB(角)){
        for(int i=xi+1,j=yi+1;i<=xj,j<=yj;i++,j++){
            if(xj==i && yj==j){
                for(int i=xi+1,j=yi+1;i<xj,j<yj;i++,j++){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi+1,j=yi-1;i<=xj,j>=yj;i++,j--){
            if(xj==i && yj==j){
                for(int i=xi+1,j=yi-1;i<xj,j>yj;i++,j--){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi-1,j=yi+1;i>=xj,j<=yj;i--,j++){
            if(xj==i && yj==j){
                for(int i=xi-1,j=yi+1;i>xj,j<yj;i--,j++){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi-1,j=yi-1;i>=xj,j>=yj;i--,j--){
            if(xj==i && yj==j){
                for(int i=xi-1,j=yi-1;i>xj,j>yj;i--,j--){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        if(max(xi,xj)-min(xi,xj)==max(yi,yj)-min(yi,yj) && isStandard && red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(角);
            upgrade();
        }
        else{
            restart =1;
        }
    }
    //chessR王將
    else if(board[xi][yi] == chessR(王)){
        if((red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2) && ((xj==xi) && (yj==yi+1 || yj==yi-1)) || ((xj==xi+1) && (yj==yi+1 || yj==yi-1 || yj==yi)) || ((xj==xi-1) && (yj==yi+1 || yj==yi-1 || yj==yi))){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(王);
        }
        else{
            restart = 1;
        }
    }
    //chessB玉將
    else if(board[xi][yi] == chessB(玉)){
         if((red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2) && ((xj==xi) && (yj==yi+1 || yj==yi-1)) || ((xj==xi+1) && (yj==yi+1 || yj==yi-1 || yj==yi)) || ((xj==xi-1) && (yj==yi+1 || yj==yi-1 || yj==yi))){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(玉);
        }
        else{
            restart = 1;
        }
    }
    //chessR龍王
    else if(board[xi][yi] == chessR(龍)){
        if(yi==yj){
            for(int i=xi+1;i<xj;i++){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
            for(int i=xi-1;i>xj;i--){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if(xi==xj){
            for(int i=yi+1;i<yj;i++){
                if(board[xi][i]!=cell(空)){
                    isStandard = 0;
                }
            }
            for(int i=xi-1;i>xj;i--){
                if(board[xi][i]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if((xi==xj || yi==yj || (max(xj,xi)-min(xj,xi)==1) || (max(yj,yi)-min(yj,yi)==1)) && isStandard && (red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(龍);
        }
        else{
            restart = 1;
        }
    }
    //chessB龍王
    else if(board[xi][yi] == chessB(龍)){
        if(yi==yj){
            for(int i=xi+1;i<xj;i++){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
            for(int i=xi-1;i>xj;i--){
                if(board[i][yi]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if(xi==xj){
            for(int i=yi+1;i<yj;i++){
                if(board[xi][i]!=cell(空)){
                    isStandard = 0;
                }
            }
            for(int i=xi-1;i>xj;i--){
                if(board[xi][i]!=cell(空)){
                    isStandard = 0;
                }
            }
        }
        if((xi==xj || yi==yj || (max(xj,xi)-min(xj,xi)==1) || (max(yj,yi)-min(yj,yi)==1)) && isStandard && (red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(龍);
        }
        else{
            restart = 1;
        }
    }
    //chessR龍馬
    else if(board[xi][yi] == chessR(馬)){
        for(int i=xi+1,j=yi+1;i<=xj,j<=yj;i++,j++){
            if(xj==i && yj==j){
                for(int i=xi+1,j=yi+1;i<xj,j<yj;i++,j++){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi+1,j=yi-1;i<=xj,j>=yj;i++,j--){
            if(xj==i && yj==j){
                for(int i=xi+1,j=yi-1;i<xj,j>yj;i++,j--){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi-1,j=yi+1;i>=xj,j<=yj;i--,j++){
            if(xj==i && yj==j){
                for(int i=xi-1,j=yi+1;i>xj,j<yj;i--,j++){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi-1,j=yi-1;i>=xj,j>=yj;i--,j--){
            if(xj==i && yj==j){
                for(int i=xi-1,j=yi-1;i>xj,j>yj;i--,j--){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        if((max(xi,xj)-min(xi,xj)==max(yi,yj)-min(yi,yj)||max(xi,xj)-min(xi,xj)==1||max(yi,yj)-min(yi,yj)==1) && isStandard && red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(馬);
        }
        else{
            restart =1;
        }
    }
    //chessB龍馬
    else if(board[xi][yi] == chessB(馬)){
        for(int i=xi+1,j=yi+1;i<=xj,j<=yj;i++,j++){
            if(xj==i && yj==j){
                for(int i=xi+1,j=yi+1;i<xj,j<yj;i++,j++){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi+1,j=yi-1;i<=xj,j>=yj;i++,j--){
            if(xj==i && yj==j){
                for(int i=xi+1,j=yi-1;i<xj,j>yj;i++,j--){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi-1,j=yi+1;i>=xj,j<=yj;i--,j++){
            if(xj==i && yj==j){
                for(int i=xi-1,j=yi+1;i>xj,j<yj;i--,j++){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        for(int i=xi-1,j=yi-1;i>=xj,j>=yj;i--,j--){
            if(xj==i && yj==j){
                for(int i=xi-1,j=yi-1;i>xj,j>yj;i--,j--){
                    if(board[i][j]!=cell(空)){
                        isStandard = 0;
                    }
                }
            }
        }
        if((max(xi,xj)-min(xi,xj)==max(yi,yj)-min(yi,yj)||max(xi,xj)-min(xi,xj)==1||max(yi,yj)-min(yi,yj)==1) && isStandard && red_or_blue(xj,yj) != -1 && red_or_blue(xj,yj) != 2){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessB(馬);
        }
        else{
            restart =1;
        }
    }
    //chessR杏、圭、と、全
    else if(board[xi][yi] == chessR(杏)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(杏);
        }
        else{
            restart = 1;
        }
    }
    else if(board[xi][yi] == chessR(圭)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(圭);
        }
        else{
            restart = 1;
        }
    }
    else if(board[xi][yi] == chessR(と)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(と);
        }
        else{
            restart = 1;
        }
    }
    else if(board[xi][yi] == chessR(全)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(全);
        }
        else{
            restart = 1;
        }
    }
    //chessB杏、圭、と、全
    else if(board[xi][yi] == chessB(杏)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(杏);
        }
        else{
            restart = 1;
        }
    }
    else if(board[xi][yi] == chessB(圭)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(圭);
        }
        else{
            restart = 1;
        }
    }
    else if(board[xi][yi] == chessB(全)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(全);
        }
        else{
            restart = 1;
        }
    }
    else if(board[xi][yi] == chessB(と)){
        if(red_or_blue(xj,yj) != 1 && red_or_blue(xj,yj) != 2 && (xj == xi+1 && yj == yi) || (xj == xi+1 && yj == yi+1) || (xj == xi+1 && yj == yi-1) || (xj == xi-1 && yj == yi) || (xj == xi && yj == yi-1) || (xj == xi && yj == yi+1)){
            captive(xj,yj);
            board[xi][yi] = cell(空);
            board[xj][yj] = chessR(と);
        }
        else{
            restart = 1;
        }
    }
    else{
        restart = 1;
    }
}
//吃掉棋子放到上手駒台或下手駒台
int captive(int x,int y){
    if((red_or_blue(x,y) == -1 )){
        for(int i=1;i<3;i++){
            for(int j=1;j<11;j++){
                if(tophand[i][j]==cell(空)){
                    tophand[i][j] = board[x][y];
                    i=3;
                    j=11;
                }
            }
        }
    }
    if((red_or_blue(x,y) == 1)){
        for(int i=0;i<2;i++){
            for(int j=1;j<11;j++){
                if(bothand[i][j]==cell(空)){
                    bothand[i][j] = board[x][y];
                    i=2;
                    j=11;
                }
            }
        }
    }
}
//升變
int upgrade(){
    char yn;
    if(red_or_blue(xj,yj)==1){
        if(xj>=7 || (xi>=7 && xj<7)){
            if(xj==9 && (board[xj][yj]==chessR(香)||board[xj][yj]==chessR(步))){
                if(board[xj][yj]==chessR(香)){
                    board[xj][yj]=chessR(杏);
                }
                else if(board[xj][yj]==chessR(步)){
                    board[xj][yj]=chessR(と);
                }
            }
            else if((xj==8 || xj==9) && board[xj][yj]==chessR(桂)){
                board[xj][yj]=chessR(圭);
            }
            else{
                printf("是否升級[y/n]: ");
                scanf(" %c",&yn);
                if(yn == 'y'){
                    if(board[xj][yj]==chessR(香)){
                        board[xj][yj]=chessR(杏);
                    }
                    else if(board[xj][yj]==chessR(銀)){
                        board[xj][yj]=chessR(全);
                    }
                    else if(board[xj][yj]==chessR(桂)){
                        board[xj][yj]=chessR(圭);
                    }
                    else if(board[xj][yj]==chessR(步)){
                        board[xj][yj]=chessR(と);
                    }
                    else if(board[xj][yj]==chessR(飛)){
                        board[xj][yj]=chessR(龍);
                    }
                    else if(board[xj][yj]==chessR(角)){
                        board[xj][yj]=chessR(馬);
                    }
                }
                else if(yn == 'n'){
                    return 0;  
                }
                else{
                    restart = 1;
                    return 0;
                }
            }
        }
    }
    if(red_or_blue(xj,yj)==-1){
        if(xj<=3 || (xi<=3 && xj>3)){
            if(xj==1 && (board[xj][yj]==chessB(香)||board[xj][yj]==chessB(步))){
                board[xj][yj]=chessB(金);
            }
            else if((xj==2 || xj==1) && board[xj][yj]==chessB(桂)){
                board[xj][yj]=chessB(金);
            }
            else{
                printf("是否升級[y/n]:");
                scanf(" %c",&yn);
                if(yn == 'y'){
                    if(board[xj][yj]==chessB(香)){
                        board[xj][yj]=chessB(杏);
                    }
                    else if(board[xj][yj]==chessB(銀)){
                        board[xj][yj]=chessB(全);
                    }
                    else if(board[xj][yj]==chessB(桂)){
                        board[xj][yj]=chessB(圭);
                    }
                    else if(board[xj][yj]==chessB(步)){
                        board[xj][yj]=chessB(と);
                    }
                    else if(board[xj][yj]==chessR(飛)){
                        board[xj][yj]=chessR(龍);
                    }
                    else if(board[xj][yj]==chessB(角)){
                        board[xj][yj]=chessB(馬);
                    }
                }
                else if(yn == 'n'){
                    return 0;  
                }
                else{
                    restart = 1;
                    return 0;
                }
            }
        }
    }
}
//棋子打入
int Turn_blueenemies_into_friends(){
    char yn;
    int count=0;
    for(int i=1;i<3;i++){
        for(int j=1;j<11;j++){
            if(tophandchek(i,j)==3){
                printf("[紅]是否選擇打入棋子[y/n]: ");
                scanf(" %c",&yn);
                if(yn=='y'){
                    printf("請輸入tophand想打入的棋子座標(行): ");
                    scanf("%d",&xi);
                    printf("請輸入tophand想打入的棋子座標(列): ");
                    scanf("%d",&yi);
                    printf("請輸入tophand想放到的棋盤座標(行): ");
                    scanf("%d",&xj);
                    printf("請輸入tophand想放到的棋盤座標(列): ");
                    scanf("%d",&yj);
                    if(xi>2 || yi>10 || xj>9 || yj>9){
                        restart = 1;
                        skip=1;
                        return 0;
                    }
                    yj=(yj-9)*(-1);
                    if(board[xj][yj]==cell(空) && (tophand[xi][yi] != cell(空)) && (tophand[xi][yi] != chessB(步)) && (tophand[xi][yi] != chessB(香)) && (tophand[xi][yi] != chessB(桂)) && (tophand[xi][yi] != chessB(圭)) && (tophand[xi][yi] != chessB(と)) && (tophand[xi][yi] != chessB(杏))){
                        skip = 1;
                        board[xj][yj] = tophand[xi][yi];
                        tophand[xi][yi] = cell(空);
                        if(board[xj][yj]==chessB(銀)){
                            board[xj][yj]=chessR(銀);
                        }
                        else if(board[xj][yj]==chessB(全)){
                            board[xj][yj]=chessR(銀);
                        }
                        else if(board[xj][yj]==chessB(金)){
                            board[xj][yj]=chessR(金);
                        }
                        else if(board[xj][yj]==chessB(飛)){
                            board[xj][yj]=chessR(飛);
                        }
                        else if(board[xj][yj]==chessB(龍)){
                            board[xj][yj]=chessR(飛);
                        }
                        else if(board[xj][yj]==chessB(角)){
                            board[xj][yj]=chessR(角);
                        }
                        else if(board[xj][yj]==chessB(馬)){
                            board[xj][yj]=chessR(飛);
                        }
                        printf("%s\n",coordin(=================================================));
                        printboard();
                    }
                    else if(board[xj][yj]==cell(空) && xj!=9 && ((tophand[xi][yi] == chessB(香)) || (tophand[xi][yi] == chessB(杏)))){
                        skip = 1;
                        board[xj][yj] = tophand[xi][yi];
                        tophand[xi][yi] = cell(空);
                        board[xj][yj]=chessR(香);
                        printf("%s\n",coordin(=================================================));
                        printboard();
                    }
                    else if(board[xj][yj]==cell(空) && xj!=9 && xj!=8 && ((tophand[xi][yi] == chessB(桂)) || (tophand[xi][yi] == chessB(圭)))){
                        skip = 1;
                        board[xj][yj] = tophand[xi][yi];
                        tophand[xi][yi] = cell(空);
                        board[xj][yj]=chessR(桂);
                        printf("%s\n",coordin(=================================================));
                        printboard();
                    }
                    else if(board[xj][yj]==cell(空) && xj!=9 && ((tophand[xi][yi] == chessB(步)) || (tophand[xi][yi] == chessB(と)))){
                        for(int i=1;i<10;i++){
                            if(board[i][yj] != chessR(步)){
                                count++;
                            }
                        }
                        if(count==9){
                            skip = 1;
                            board[xj][yj] = tophand[xi][yi];
                            tophand[xi][yi] = cell(空);
                            board[xj][yj]=chessR(步);
                            printf("%s\n",coordin(=================================================));
                            printboard();
                        }
                    }
                    else{
                        skip = 1;
                        restart = 1;
                    }
                    return 0;
                }
                else if(yn == 'n'){
                    return 0;  
                }
                else{
                    restart = 1;
                    return 0;
                }
            }
        }
    }
}
int Turn_redenemies_into_friends(){
    char yn;
    int count=0;
    for(int i=0;i<2;i++){
        for(int j=1;j<11;j++){
            if(bothandchek(i,j) == 4){
                printf("[藍]是否選擇打入棋子[y/n]: ");
                scanf(" %c",&yn);
                if(yn=='y'){
                    printf("請輸入bothand想打入的棋子座標(行): ");
                    scanf("%d",&xi);
                    printf("請輸入bothand想打入的棋子座標(列): ");
                    scanf("%d",&yi);
                    printf("請輸入bothand想放到的棋盤座標(行): ");
                    scanf("%d",&xj);
                    printf("請輸入bothand想放到的棋盤座標(列): ");
                    scanf("%d",&yj);
                    if(xi>2 || yi>10 || xj>9 || yj>9){
                        restart = 1;
                        skip=1;
                        return 0;
                    }
                    xi-=1;
                    yj=(yj-9)*(-1);
                    if(board[xj][yj]==cell(空) && (bothand[xi][yi] != cell(空)) && (bothand[xi][yi] != chessR(步)) && (bothand[xi][yi] != chessR(香)) && (bothand[xi][yi] != chessR(桂)) && (bothand[xi][yi] != chessR(圭)) && (bothand[xi][yi] != chessR(と)) && (bothand[xi][yi] != chessR(杏))){
                        skip = 1;
                        board[xj][yj] = bothand[xi][yi];
                        bothand[xi][yi] = cell(空);
                        if(board[xj][yj]==chessR(銀)){
                            board[xj][yj]=chessB(銀);
                        }
                        else if(board[xj][yj]==chessR(全)){
                            board[xj][yj]=chessB(銀);
                        }
                        else if(board[xj][yj]==chessR(金)){
                            board[xj][yj]=chessB(金);
                        }
                        else if(board[xj][yj]==chessR(飛)){
                            board[xj][yj]=chessB(飛);
                        }
                        else if(board[xj][yj]==chessR(龍)){
                            board[xj][yj]=chessB(飛);
                        }
                        else if(board[xj][yj]==chessR(角)){
                            board[xj][yj]=chessB(角);
                        }
                        else if(board[xj][yj]==chessR(馬)){
                            board[xj][yj]=chessB(飛);
                        }
                        printf("%s\n",coordin(=================================================));
                        printboard();
                    }
                    else if(board[xj][yj]==cell(空) && xj!=1 && (bothand[xi][yi] == chessR(香)) || (bothand[xi][yi] == chessR(杏))){
                        skip = 1;
                        board[xj][yj] = bothand[xi][yi];
                        bothand[xi][yi] = cell(空);
                        board[xj][yj]=chessB(香);
                        printf("%s\n",coordin(=================================================));
                        printboard();
                    }
                    else if(board[xj][yj]==cell(空) && xj!=1 && xj!=2 && (bothand[xi][yi] == chessR(桂)|| bothand[xi][yi] == chessR(圭))){
                        skip = 1;
                        board[xj][yj] = bothand[xi][yi];
                        bothand[xi][yi] = cell(空);
                        board[xj][yj]=chessB(桂);
                        printf("%s\n",coordin(=================================================));
                        printboard();
                    }
                    else if(board[xj][yj]==cell(空) && xj!=1 && (bothand[xi][yi] == chessR(步) || bothand[xi][yi] == chessR(と))){
                        for(int i=9;i>0;i--){
                            if(board[i][yj] != chessB(步)){
                                count++;
                            }
                        }
                        if(count==9){
                            skip = 1;
                            board[xj][yj] = bothand[xi][yi];
                            bothand[xi][yi] = cell(空);
                            board[xj][yj]=chessB(步);
                            printf("%s\n",coordin(=================================================));
                            printboard();
                        }
                    }
                    else{
                        skip = 1;
                        restart = 1;
                    }
                    return 0;
                }
                else if(yn == 'n'){
                    return 0;  
                }
                else{
                    restart = 1;
                    return 0;
                }
            }
        }
    }
}
//新增一個儲存棋盤的陣列
board_save *CreateSavedBoard(board_save *list)
{
    board_save *tmp;
    tmp = (board_save*)malloc(sizeof(board_save));
    list->right = tmp;      //原本的接到tmp
    tmp->left = list;      //左邊接到之前
    list = tmp;             //list_of_board 換到下一個

    return(list);
}
//儲存棋盤和換誰下棋
void SavedBoard(board_save *list, int turn)
{
    for(int i = 0; i <3;i++){
        for(int j = 0; j<14;j++){
            list->saved_tophand[i][j] = tophand[i][j];
            list->saved_bothand[i][j] = bothand[i][j];
        }
    }
    for(int i = 0; i <10;i++){
        for(int j = 0; j <10;j++){
            list->saved_board[i][j] = board[i][j];
        }
    }
    list->saved_red_or_blue = turn;
}
//更新board並退回到上一個linklist
board_save *Back(board_save *list, int turn)
{
    if(list->left != NULL){
        board_save *tmp;   
        tmp = list;
        list = list->left;    //  退到上一個
        for(int i = 0; i < 3;i++){
            for(int j = 0;j < 14;j++){
                tophand[i][j] = list->saved_tophand[i][j];
                bothand[i][j] = list->saved_bothand[i][j];
            }
        }
        for(int i = 0; i < 10;i++){
            for(int j = 0; j < 10;j++){
                board[i][j] = list->saved_board[i][j];
            }
        }
        turn = list->saved_red_or_blue;
        free(tmp);
        return(list);
    }
}
//到下一個linklist(無作用)
board_save *Next(board_save *list, int turn)
{
    if(list->right != NULL){
        board_save *tmp;
        tmp = list;
        list = list->right;
        for(int i = 0; i < 3;i++){
            for(int j = 0;j < 14;j++){
                tophand[i][j] = list->saved_tophand[i][j];
                bothand[i][j] = list->saved_bothand[i][j];
            }
        }
        for(int i = 0; i < 10;i++){
            for(int j = 0; j < 10;j++){
                board[i][j] = list->saved_board[i][j];
            }
        }
        turn = list->saved_red_or_blue;
        free(tmp);
        return(list);
    }
}


//計時
static void io_cb(EV_P_ ev_io *w, int revents)
{
	ev_io_stop(EV_A_ w);
    ev_timer_stop(loop, &time_watcher);
	ev_break(loop, EVBREAK_ALL);
}
static void timer_cb(EV_P_ ev_timer *w, int revents)
{
    if(turn == 1){
        t2 = t2+1;
    }
    else if(turn == -1){
        t1 = t1+1;
    }
    system("clear");
    printf("時間[紅]:%ld ,時間[藍]:%ld\n",t2,t1);
    printboard();
    fflush(stdout);
}
void timemain()
{
  struct ev_loop *loop = EV_DEFAULT;
  ev_io_init(&io_watcher,io_cb,0,EV_READ);
  ev_io_start(loop, &io_watcher);
  ev_timer_init(&time_watcher,timer_cb,0,1);
  ev_timer_start(loop, &time_watcher);
  ev_run(loop, 0);
}
