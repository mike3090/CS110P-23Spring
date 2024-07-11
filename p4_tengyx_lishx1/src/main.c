#include "lcd/lcd.h"
#include <string.h>
#include "utils.h"
#include "welcome_face.h"
#include "img.h"
#include "gameboards.h"


// Actually a better data structure (2D matrix) can be used here.
// However it's an one-time code!
int Best3_1Box[3] = {2147483647,2147483647,2147483647};
int Best3_2Box[3] = {2147483647,2147483647,2147483647};
int Best3_3Box[3] = {2147483647,2147483647,2147483647};

// 0 for empty, 1 for player, 2 for box_unsuccess, 3 for wall, 
// 4 for dest, 5 for box_success, 6 for player on the dest
int gameboard[8][20]={0};



void Inp_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOC);

    gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_6);
    gpio_init(GPIOC, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
}

void IO_init(void)
{
    Inp_init(); // inport init
    Lcd_Init(); // LCD init
}

void my_lcd_show_picture(u16 x1, u16 y1, u16 x2, u16 y2, unsigned char img[]) {
    int size = (x2 - x1 + 1) * (y2 - y1 + 1) * 2;
    LCD_Address_Set(x1, y1, x2, y2);
    for (int i = 0; i < size; i++) {
        LCD_WR_DATA8(img[i]);
    }
}

uint32_t numlen(uint32_t num)
{   
    if(num==0){return 1;}
    uint32_t len = 0;        // 初始长度为0
    for(; num > 0; ++len)    // 判断num是否大于0，否则长度+1
        num /= 10;	         // 使用除法进行运算，直到num小于1
    return len;              // 返回长度的值
}

void welcomeInterface(void){
    my_lcd_show_picture(0,0,159,79,welcome);
}

// I would prefer arduino-style delay()...
void delay(uint32_t x){
    delay_1ms(x);
}

void levelSelection();


void scoreboard(int level){
    LCD_Clear(0x0000);
    while(1){
        if(level==1){
            while(1){
                LCD_ShowString(20,0,"Scoreboard: 1Box", 0xffff);
                LCD_ShowString(24,16,"1st",YELLOW);
                LCD_ShowString(24,32,"2nd",GRAY);
                LCD_ShowString(24,48,"3rd",BROWN);
                LCD_ShowNum(56,16,Best3_1Box[0],numlen(Best3_1Box[0]),YELLOW);
                LCD_ShowNum(56,32,Best3_1Box[1],numlen(Best3_1Box[1]),GRAY);
                LCD_ShowNum(56,48,Best3_1Box[2],numlen(Best3_1Box[2]),BROWN);
                LCD_ShowString(32,64,"back: CTR/SW1>>",0xffff);
                if(Get_Button(BUTTON_1)||Get_Button(JOY_CTR)){
                    LCD_Clear(0x0000);
                    return;
                }
                if(Get_Button(JOY_LEFT)){
                    LCD_Fill(56,16,135,63,0x0000);
                    level=3;
                    break;
                }
                if(Get_Button(JOY_RIGHT)){
                    LCD_Fill(56,16,135,63,0x0000);
                    level=2;
                    break;
                }
                delay(200);
            }
        }
        else if(level==2){
            while(1){
                LCD_ShowString(20,0,"Scoreboard: 2Box", 0xffff);
                LCD_ShowString(24,16,"1st",YELLOW);
                LCD_ShowString(24,32,"2nd",GRAY);
                LCD_ShowString(24,48,"3rd",BROWN);
                LCD_ShowNum(56,16,Best3_2Box[0],numlen(Best3_2Box[0]),YELLOW);
                LCD_ShowNum(56,32,Best3_2Box[1],numlen(Best3_2Box[1]),GRAY);
                LCD_ShowNum(56,48,Best3_2Box[2],numlen(Best3_2Box[2]),BROWN);
                LCD_ShowString(32,64,"back: CTR/SW1>>",0xffff);
                if(Get_Button(BUTTON_1)||Get_Button(JOY_CTR)){
                    LCD_Clear(0x0000);
                    return;
                }
                if(Get_Button(JOY_LEFT)){
                    LCD_Fill(56,16,135,63,0x0000);
                    level=1;
                    break;
                }
                if(Get_Button(JOY_RIGHT)){
                    LCD_Fill(56,16,135,63,0x0000);
                    level=3;
                    break;
                }
                delay(200);
            }
        }
        else if(level==3){
            while(1){
                LCD_ShowString(20,0,"Scoreboard: 3Box", 0xffff);
                LCD_ShowString(24,16,"1st",YELLOW);
                LCD_ShowString(24,32,"2nd",GRAY);
                LCD_ShowString(24,48,"3rd",BROWN);
                LCD_ShowNum(56,16,Best3_3Box[0],numlen(Best3_3Box[0]),YELLOW);
                LCD_ShowNum(56,32,Best3_3Box[1],numlen(Best3_3Box[1]),GRAY);
                LCD_ShowNum(56,48,Best3_3Box[2],numlen(Best3_3Box[2]),BROWN);
                LCD_ShowString(32,64,"back: CTR/SW1>>",0xffff);
                if(Get_Button(BUTTON_1)||Get_Button(JOY_CTR)){
                    LCD_Clear(0x0000);
                    return;
                }
                if(Get_Button(JOY_LEFT)){
                    LCD_Fill(56,16,135,63,0x0000);
                    level=2;
                    break;
                }
                if(Get_Button(JOY_RIGHT)){
                    LCD_Fill(56,16,135,63,0x0000);
                    level=1;
                    break;
                }
                delay(200);
            }
        }
        delay(200);
    }
    
    
}

int main(void)
{
    IO_init();         // init OLED
    LCD_Clear(0x0000);
    // YOUR CODE HERE
    // Welcome page.
    while(1){
        welcomeInterface();
        if(Get_BOOT0()){
            // jump out, to the menu
            break;
        }
    }
    //delay(300);
    LCD_Clear(0x0000);
    // Menu
    int whichSelect = 0; // 0 for LevelSelection, 1 for Scoreboard
    while(1){
        // LCD_Clear(0x0000);
        LCD_ShowString(48,8,"Sokoban!",0xffff);
        
        if(whichSelect==0){
            LCD_Fill(26,52,34,60,0x0000);
            LCD_DrawCircle(10,40,4,0xFFE0);
            LCD_ShowString(20,32,"Level Selection", 0xFFE0);
            LCD_ShowString(40,48,"Scoreboard", 0xffff);
        }
        else{
            LCD_Fill(6,36,14,44,0x0000);
            LCD_DrawCircle(30,56,4,0xFFE0);
            LCD_ShowString(20,32,"Level Selection", 0xffff);
            LCD_ShowString(40,48,"Scoreboard", 0xFFE0);
        }

        if(Get_Button(JOY_DOWN)){
            whichSelect = 1 - whichSelect;
            
        }
        
        // jump to corresponding pages
        if(Get_Button(JOY_CTR) || Get_BOOT0()){
            if(whichSelect==0){
                levelSelection(1);
            }
            else{
                scoreboard(1);
            }
        }
        delay(200);
    }
    
}

int game(int boxNumber);

void levelSelection(){
    LCD_Clear(0x0000);
    int boxNumber = 1;
    while(1){
        LCD_ShowString(36,24,"Level", 0xffff);
        LCD_ShowString(20,40,"Selection", 0xffff);
        LCD_ShowString(108,8,"Boxes",0xffff);
        if(boxNumber == 1){
            LCD_Fill(116,60,124,68,0x0000);
            LCD_DrawCircle(120,32,4,0xFFE0);
            LCD_ShowString(132,24,"1", 0xFFE0);
            LCD_ShowString(132,40,"2", 0xffff);
            LCD_ShowString(132,56,"3", 0xffff);
           
        }
        else if(boxNumber == 2){
            LCD_Fill(116,28,124,36,0x0000);
            LCD_DrawCircle(120,48,4,0xFFE0);
            LCD_ShowString(132,24,"1", 0xffff);
            LCD_ShowString(132,40,"2", 0xFFE0);
            LCD_ShowString(132,56,"3", 0xffff);
        }
        else if(boxNumber == 3){
            LCD_Fill(116,44,124,52,0x0000);
            LCD_DrawCircle(120,64,4,0xFFE0);
            LCD_ShowString(132,24,"1", 0xffff);
            LCD_ShowString(132,40,"2", 0xffff);
            LCD_ShowString(132,56,"3", 0xFFE0);
        }

        if(Get_Button(JOY_DOWN)){
            if(boxNumber==3){boxNumber=1;}
            else{boxNumber++;}            
        }
        if(Get_Button(JOY_CTR) || Get_BOOT0()){
            int gamesuccess = game(boxNumber);
            if(gamesuccess==0){
                scoreboard(boxNumber);
            }
            else if(gamesuccess==1){
                ;
            }
            
        }
        if(Get_Button(BUTTON_1)){
            LCD_Clear(0x0000);
            return;
        }
        delay(200);
    }
}

void showNewRecord(int score){
    LCD_ShowString(68,0,"N",YELLOW);
    LCD_ShowString(76,0,"E",GREEN);
    LCD_ShowString(84,0,"W",0x07ff);
    
    LCD_ShowString(52,16,"R",RED);
    LCD_ShowString(60,16,"E",0xfd20);
    LCD_ShowString(68,16,"C",YELLOW);
    LCD_ShowString(76,16,"O",GREEN);
    LCD_ShowString(84,16,"R",0x07ff);
    LCD_ShowString(92,16,"D",BLUE);
    LCD_ShowString(100,16,"!",0x8010);

    my_lcd_show_picture(24,12,31,19,heart);
    my_lcd_show_picture(128,12,135,19,heart);

    LCD_ShowString(16,40,"Your Score: ",0xffff);
    LCD_ShowNum(112,40,score,numlen(score),0xffff);
    
    LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
}


// return 0 if successfully finished the game,
// return 1 if SW1 is pressed to emergently exit
int game(int boxNumber){
    int successBoxNum = 0;
    LCD_Clear(0x0000);
    LCD_ShowString(52,32,"Level ", 0xffff);
    LCD_ShowNum(100,32,boxNumber,1,0xffe0);
    delay(1000);
    LCD_Clear(0x0000);
    int moveNumber = 0;
    int playerX=0; // 横着, 0-19
    int playerY=0; // 竖着, 0-7
    // get the gameboard
    LCD_ShowString(0,0,"Moves: ",0xffff);
    LCD_ShowNum(56,0,moveNumber,numlen(moveNumber),0xffff);
    LCD_ShowString(96,0,"CTR: Up",0xffff);
    if(boxNumber==1){
        memcpy(gameboard, board1, 20*8*sizeof(int));
    }
    else if(boxNumber==2){
        memcpy(gameboard, board2, 20*8*sizeof(int));
    }
    else if(boxNumber==3){
        memcpy(gameboard, board3, 20*8*sizeof(int));
    }
    // 0 for empty, 1 for player, 2 for box_unsuccess, 3 for wall,
    // 4 for dest, 5 for box_success, 6 for player on the dest
    // we don't design the case: at init, a box is on dest
    for(int i=0;i<8;i++){
        for(int j=0;j<20;j++){
            if(gameboard[i][j]==0){
                my_lcd_show_picture(j*8,16+i*8,j*8+7,16+i*8+7,empty);
            }
            if(gameboard[i][j]==1||gameboard[i][j]==6){
                my_lcd_show_picture(j*8,16+i*8,j*8+7,16+i*8+7,player);
                playerX=j;
                playerY=i;
            }
            if(gameboard[i][j]==2){
                my_lcd_show_picture(j*8,16+i*8,j*8+7,16+i*8+7,box_unsuccess);
            }
            if(gameboard[i][j]==3){
                my_lcd_show_picture(j*8,16+i*8,j*8+7,16+i*8+7,wallBrick);
            }
            if(gameboard[i][j]==4){
                my_lcd_show_picture(j*8,16+i*8,j*8+7,16+i*8+7,destination);
            }
            // but we don't need to care 5 at init
        }
    }
    // main loop
    while(1){

        // LET THE BOARD SURROUNDED BY BRICKS.
        // I don't want to do any boarder-check here...
        // However I implemented them. Thank you TENG YUEXIN :)

        // Judge if success
        if(boxNumber==successBoxNum){
            break;
        }

        if(Get_Button(JOY_CTR)){ // up
            // if at upper boundary
            if(playerY==0){
                delay(300);
                continue;
            }
            // if upper is wall
            if(gameboard[playerY-1][playerX]==3){
                delay(300);
                continue;
            }
            // if upper is box
            if(gameboard[playerY-1][playerX]==2 || gameboard[playerY-1][playerX]==5){
                // if box is at boundary
                if(playerY==1){
                    delay(300);
                    continue;
                }
                else{
                    // if upper-upper is wall or box
                    if(gameboard[playerY-2][playerX]==3||gameboard[playerY-2][playerX]==2||gameboard[playerY-2][playerX]==5){
                        delay(300);
                        continue;   
                    }

                    // if now box is on dest, we push it, it will be not on dest
                    if(gameboard[playerY-1][playerX]==5){
                        successBoxNum--;
                    }

                    // if upper-upper is dest
                    if(gameboard[playerY-2][playerX]==4){
                        gameboard[playerY-2][playerX]=5;
                        successBoxNum++;
                        my_lcd_show_picture((playerX)*8,16+(playerY-2)*8,(playerX)*8+7,16+(playerY-2)*8+7,box_success);    
                    }
                    else{
                        gameboard[playerY-2][playerX]=2;
                        my_lcd_show_picture((playerX)*8,16+(playerY-2)*8,(playerX)*8+7,16+(playerY-2)*8+7,box_unsuccess);
                    }

                    // bugs found when pushing an success box to a dest
                    if(gameboard[playerY-1][playerX]==5){
                        gameboard[playerY-1][playerX]=6;
                    }
                    else{
                        gameboard[playerY-1][playerX]=1;
                    }
                    
                    my_lcd_show_picture((playerX)*8,16+(playerY-1)*8,(playerX)*8+7,16+(playerY-1)*8+7,player);

                    // if originally player is standing on dest
                    if(gameboard[playerY][playerX]==6){
                        gameboard[playerY][playerX]=4;
                        my_lcd_show_picture((playerX)*8,16+(playerY)*8,(playerX)*8+7,16+(playerY)*8+7,destination);
                    }
                    else{
                        gameboard[playerY][playerX]=0;
                        my_lcd_show_picture((playerX)*8,16+(playerY)*8,(playerX)*8+7,16+(playerY)*8+7,empty);
                    }
                    playerY--;
                    moveNumber++;
                    LCD_Fill(56,0,95,15,0x0000);
                    LCD_ShowNum(56,0,moveNumber,numlen(moveNumber),0xffff);
                    delay(300);
                    continue;
                }
            }
            // if upper isn't wall or box(is empty or dest), then move!
            else{
                
                // if upper is dest
                if(gameboard[playerY-1][playerX]==4){
                    gameboard[playerY-1][playerX]=6;
                }
                // empty
                else{
                    gameboard[playerY-1][playerX]=1;
                }
                my_lcd_show_picture((playerX)*8,16+(playerY-1)*8,(playerX)*8+7,16+(playerY-1)*8+7,player);
                // if originally player is standing on dest
                if(gameboard[playerY][playerX]==6){
                    gameboard[playerY][playerX]=4;
                    my_lcd_show_picture((playerX)*8,16+(playerY)*8,(playerX)*8+7,16+(playerY)*8+7,destination);
                }
                else{
                    gameboard[playerY][playerX]=0;
                    my_lcd_show_picture((playerX)*8,16+(playerY)*8,(playerX)*8+7,16+(playerY)*8+7,empty);
                }
            }
            playerY--;
            moveNumber++;
            LCD_Fill(56,0,95,15,0x0000);
            LCD_ShowNum(56,0,moveNumber,numlen(moveNumber),0xffff);
            delay(300);
            continue;
        }
        else if(Get_Button(JOY_DOWN)){ // down
            // instead of playerY-2, Y-1, Y,
            // it would be Y+2, Y+1, Y 
            // border: 6/7
            // if at lower boundary
            if (playerY == 7) {
                delay(300);
                continue;
            }
            // if lower is wall
            if (gameboard[playerY + 1][playerX] == 3) {
                delay(300);
                continue;
            }
            // if lower is box
            if (gameboard[playerY + 1][playerX] == 2 || gameboard[playerY + 1][playerX] == 5) {
                // if box is at boundary
                if (playerY == 6) {
                    delay(300);
                    continue;
                }
                else {
                    // if lower-lower is wall or box
                    if (gameboard[playerY + 2][playerX] == 3 || 
                        gameboard[playerY + 2][playerX] == 2 || gameboard[playerY + 2][playerX] == 5) {
                        delay(300);
                        continue;
                    }
                    // if now box is on dest, we push it, it will be not on dest
                    if (gameboard[playerY + 1][playerX] == 5) {
                        successBoxNum--;
                    }
                    // if lower-lower is dest
                    if (gameboard[playerY + 2][playerX] == 4) {
                        gameboard[playerY + 2][playerX] = 5;
                        successBoxNum++;
                        my_lcd_show_picture((playerX) * 8, 16 + (playerY + 2) * 8, (playerX) * 8 + 7, 16 + (playerY + 2) * 8 + 7, box_success);
                    }
                    else {
                        gameboard[playerY + 2][playerX] = 2;
                        my_lcd_show_picture((playerX) * 8, 16 + (playerY + 2) * 8, (playerX) * 8 + 7, 16 + (playerY + 2) * 8 + 7, box_unsuccess);
                    }

                    // bugs found when pushing an success box to a dest
                    if(gameboard[playerY+1][playerX]==5){
                        gameboard[playerY+1][playerX]=6;
                    }
                    else{
                        gameboard[playerY+1][playerX]=1;
                    }

                    my_lcd_show_picture((playerX) * 8, 16 + (playerY + 1) * 8, (playerX) * 8 + 7, 16 + (playerY + 1) * 8 + 7, player);
                    // if originally player is standing on dest
                    if (gameboard[playerY][playerX] == 6) {
                        gameboard[playerY][playerX] = 4;
                        my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, destination);
                    }
                    else {
                        gameboard[playerY][playerX] = 0;
                        my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, empty);
                    }
                    playerY++;
                    moveNumber++;
                    LCD_Fill(56, 0, 95, 15, 0x0000);
                    LCD_ShowNum(56, 0, moveNumber, numlen(moveNumber), 0xffff);
                    delay(300);
                    continue;
                }
            }
            // if lower isn't wall or box(is empty or dest), then move!
            else {
                // if lower is dest
                if (gameboard[playerY + 1][playerX] == 4) {
                    gameboard[playerY + 1][playerX] = 6;
                }
                // empty
                else {
                    gameboard[playerY + 1][playerX] = 1;
                }
                my_lcd_show_picture((playerX) * 8, 16 + (playerY + 1) * 8, (playerX) * 8 + 7, 16 + (playerY + 1) * 8 + 7, player);
                // if originally player is standing on dest
                if (gameboard[playerY][playerX] == 6) {
                    gameboard[playerY][playerX] = 4;
                    my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, destination);
                }
                else {
                    gameboard[playerY][playerX] = 0;
                    my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, empty);
                }
            }
            playerY++;
            moveNumber++;
            LCD_Fill(56, 0, 95, 15, 0x0000);
            LCD_ShowNum(56, 0, moveNumber, numlen(moveNumber), 0xffff);
            delay(300);
            continue;
        }
        else if(Get_Button(JOY_LEFT)){ // left
            // instead of playerY-2, Y-1, Y,
            // Don't change playerY! change X please
            // it would be X-2, X-1, X 
            // border: 0/1
            // if at left boundary
            if (playerX == 0) {
                delay(300);
                continue;
            }
            // if left is wall
            if (gameboard[playerY][playerX - 1] == 3) {
                delay(300);
                continue;
            }
            // if left is box
            if (gameboard[playerY][playerX - 1] == 2 || gameboard[playerY][playerX - 1] == 5) {
                // if box is at boundary
                if (playerX == 1) {
                    delay(300);
                    continue;
                }
                else {
                    // if left-left is wall or box
                    if (gameboard[playerY][playerX - 2] == 3 || 
                        gameboard[playerY][playerX - 2] == 2 || gameboard[playerY][playerX - 2] == 5) {
                        delay(300);
                        continue;
                    }
                    // if now box is on dest, we push it, it will be not on dest
                    if (gameboard[playerY][playerX - 1] == 5) {
                        successBoxNum--;
                    }
                    // if left-left is dest
                    if (gameboard[playerY][playerX - 2] == 4) {
                        gameboard[playerY][playerX - 2] = 5;
                        successBoxNum++;
                        my_lcd_show_picture((playerX - 2) * 8, 16 + (playerY) * 8, (playerX - 2) * 8 + 7, 16 + (playerY) * 8 + 7, box_success);
                    }
                    else {
                        gameboard[playerY][playerX - 2] = 2;
                        my_lcd_show_picture((playerX - 2) * 8, 16 + (playerY) * 8, (playerX - 2) * 8 + 7, 16 + (playerY) * 8 + 7, box_unsuccess);
                    }

                    // bugs found when pushing an success box to a dest
                    if(gameboard[playerY][playerX - 1]==5){
                        gameboard[playerY][playerX - 1]=6;
                    }
                    else{
                        gameboard[playerY][playerX - 1]=1;
                    }

                    my_lcd_show_picture((playerX - 1) * 8, 16 + (playerY) * 8, (playerX - 1) * 8 + 7, 16 + (playerY) * 8 + 7, player);
                    // if originally player is standing on dest
                    if (gameboard[playerY][playerX] == 6) {
                        gameboard[playerY][playerX] = 4;
                        my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, destination);
                    }
                    else {
                        gameboard[playerY][playerX] = 0;
                        my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, empty);
                    }
                    playerX--;
                    moveNumber++;
                    LCD_Fill(56, 0, 95, 15, 0x0000);
                    LCD_ShowNum(56, 0, moveNumber, numlen(moveNumber), 0xffff);
                    delay(300);
                    continue;
                }
            }
            // if left isn't wall or box(is empty or dest), then move!
            else {
                // if left is dest
                if (gameboard[playerY][playerX - 1] == 4) {
                    gameboard[playerY][playerX - 1] = 6;
                }
                // empty
                else {
                    gameboard[playerY][playerX - 1] = 1;
                }
                my_lcd_show_picture((playerX - 1) * 8, 16 + (playerY) * 8, (playerX - 1) * 8 + 7, 16 + (playerY) * 8 + 7, player);
                // if originally player is standing on dest
                if (gameboard[playerY][playerX] == 6) {
                    gameboard[playerY][playerX] = 4;
                    my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, destination);
                }
                else {
                    gameboard[playerY][playerX] = 0;
                    my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, empty);
                }
            }
            playerX--;
            moveNumber++;
            LCD_Fill(56, 0, 95, 15, 0x0000);
            LCD_ShowNum(56, 0, moveNumber, numlen(moveNumber), 0xffff);
            delay(300);
            continue;
        }
        else if(Get_Button(JOY_RIGHT)){ // right
            // instead of playerX-2, X-1, X,
            // it would be X+2, X+1, X
            // border: 18/19
            // if at right boundary
            if (playerX == 19) {
                delay(300);
                continue;
            }
            // if right is wall
            if (gameboard[playerY][playerX + 1] == 3) {
                delay(300);
                continue;
            }
            // if right is box
            if (gameboard[playerY][playerX + 1] == 2 || gameboard[playerY][playerX + 1] == 5) {
                // if box is at boundary
                if (playerX == 18) {
                    delay(300);
                    continue;
                }
                else {
                    // if right-right is wall or box
                    if (gameboard[playerY][playerX + 2] == 3 || 
                        gameboard[playerY][playerX + 2] == 2 || gameboard[playerY][playerX + 2] == 5) {
                        delay(300);
                        continue;
                    }
                    // if now box is on dest, we push it, it will be not on dest
                    if (gameboard[playerY][playerX + 1] == 5) {
                        successBoxNum--;
                    }
                    // if right-right is dest
                    if (gameboard[playerY][playerX + 2] == 4) {
                        gameboard[playerY][playerX + 2] = 5;
                        successBoxNum++;
                        my_lcd_show_picture((playerX + 2) * 8, 16 + (playerY) * 8, (playerX + 2) * 8 + 7, 16 + (playerY) * 8 + 7, box_success);
                    }
                    else {
                        gameboard[playerY][playerX + 2] = 2;
                        my_lcd_show_picture((playerX + 2) * 8, 16 + (playerY) * 8, (playerX + 2) * 8 + 7, 16 + (playerY) * 8 + 7, box_unsuccess);
                    }
                    // bugs found when pushing an success box to a dest
                    if(gameboard[playerY][playerX + 1]==5){
                        gameboard[playerY][playerX + 1]=6;
                    }
                    else{
                        gameboard[playerY][playerX + 1]=1;
                    }

                    my_lcd_show_picture((playerX + 1) * 8, 16 + (playerY) * 8, (playerX + 1) * 8 + 7, 16 + (playerY) * 8 + 7, player);
                    // if originally player is standing on dest
                    if (gameboard[playerY][playerX] == 6) {
                        gameboard[playerY][playerX] = 4;
                        my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, destination);
                    }
                    else {
                        gameboard[playerY][playerX] = 0;
                        my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, empty);
                    }
                    playerX++;
                    moveNumber++;
                    LCD_Fill(56, 0, 95, 15, 0x0000);
                    LCD_ShowNum(56, 0, moveNumber, numlen(moveNumber), 0xffff);
                    delay(300);
                    continue;
                }
            }
            // if right isn't wall or box(is empty or dest), then move!
            else {
                // if right is dest
                if (gameboard[playerY][playerX + 1] == 4) {
                    gameboard[playerY][playerX + 1] = 6;
                }
                // empty
                else {
                    gameboard[playerY][playerX + 1] = 1;
                }
                my_lcd_show_picture((playerX + 1) * 8, 16 + (playerY) * 8, (playerX + 1) * 8 + 7, 16 + (playerY) * 8 + 7, player);
                // if originally player is standing on dest
                if (gameboard[playerY][playerX] == 6) {
                    gameboard[playerY][playerX] = 4;
                    my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, destination);
                }
                else {
                    gameboard[playerY][playerX] = 0;
                    my_lcd_show_picture((playerX) * 8, 16 + (playerY) * 8, (playerX) * 8 + 7, 16 + (playerY) * 8 + 7, empty);
                }
            }
            playerX++;
            moveNumber++;
            LCD_Fill(56, 0, 95, 15, 0x0000);
            LCD_ShowNum(56, 0, moveNumber, numlen(moveNumber), 0xffff);
            delay(300);
            continue;
        }
        else if(Get_Button(BUTTON_1)){ // emergently exit
            LCD_Clear(0x0000);
            delay(300);
            return 1;
        }
        delay(200);
    }
    // if success
    LCD_Clear(0x0000);
    while(1){
        if(boxNumber == 1){
            if(moveNumber <= Best3_1Box[0]){
                if(moveNumber < Best3_1Box[0]){
                    showNewRecord(moveNumber);
                }
                else{
                    LCD_ShowString(56,8,"PRIZE!",0xffff);
                    LCD_ShowString(16,32,"Your Score: ",0xffff);
                    LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                    LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff); 
                }
                Best3_1Box[2]=Best3_1Box[1];
                Best3_1Box[1]=Best3_1Box[0];
                Best3_1Box[0]=moveNumber;             
            }
            else if(Best3_1Box[0] < moveNumber && moveNumber <= Best3_1Box[1]){
                LCD_ShowString(56,8,"PRIZE!",0xffff);
                LCD_ShowString(16,32,"Your Score: ",0xffff);
                LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
                Best3_1Box[2]=Best3_1Box[1];
                Best3_1Box[1]=moveNumber;
            }
            else if(Best3_1Box[1] < moveNumber && moveNumber <= Best3_1Box[2]){
                LCD_ShowString(56,8,"PRIZE!",0xffff);
                LCD_ShowString(16,32,"Your Score: ",0xffff);
                LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
                Best3_1Box[2]=moveNumber; 
            }
            else{
                LCD_ShowString(52,8,"FINISH!",0xffff);
                LCD_ShowString(16,32,"Your Score: ",0xffff);
                LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
            }
            while(1){
                if(Get_Button(JOY_CTR)){
                    return 0;
                }
                delay(200);
            }
        }
        else if(boxNumber == 2){
            if(moveNumber <= Best3_2Box[0]){
                if(moveNumber < Best3_2Box[0]){
                    showNewRecord(moveNumber);
                }
                else{
                    LCD_ShowString(56,8,"PRIZE!",0xffff);
                    LCD_ShowString(16,32,"Your Score: ",0xffff);
                    LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                    LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff); 
                }
                Best3_2Box[2]=Best3_2Box[1];
                Best3_2Box[1]=Best3_2Box[0];
                Best3_2Box[0]=moveNumber;             
            }
            else if(Best3_2Box[0] < moveNumber && moveNumber <= Best3_2Box[1]){
                LCD_ShowString(56,8,"PRIZE!",0xffff);
                LCD_ShowString(16,32,"Your Score: ",0xffff);
                LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
                Best3_2Box[2]=Best3_2Box[1];
                Best3_2Box[1]=moveNumber;
            }
            else if(Best3_2Box[1] < moveNumber && moveNumber <= Best3_2Box[2]){
                LCD_ShowString(56,8,"PRIZE!",0xffff);
                LCD_ShowString(16,32,"Your Score: ",0xffff);
                LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
                Best3_2Box[2]=moveNumber; 
            }
            else{
                LCD_ShowString(52,8,"FINISH!",0xffff);
                LCD_ShowString(16,32,"Your Score: ",0xffff);
                LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
            }
            while(1){
                if(Get_Button(JOY_CTR)){
                    return 0;
                }
                delay(200);
            }
        }
        else if(boxNumber == 3){
            if(moveNumber <= Best3_3Box[0]){
                if(moveNumber < Best3_3Box[0]){
                    showNewRecord(moveNumber);
                }
                else{
                    LCD_ShowString(56,8,"PRIZE!",0xffff);
                    LCD_ShowString(16,32,"Your Score: ",0xffff);
                    LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                    LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff); 
                }
                Best3_3Box[2]=Best3_3Box[1];
                Best3_3Box[1]=Best3_3Box[0];
                Best3_3Box[0]=moveNumber;             
            }
            else if(Best3_3Box[0] < moveNumber && moveNumber <= Best3_3Box[1]){
                LCD_ShowString(56,8,"PRIZE!",0xffff);
                LCD_ShowString(16,32,"Your Score: ",0xffff);
                LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
                Best3_3Box[2]=Best3_3Box[1];
                Best3_3Box[1]=moveNumber;
            }
            else if(Best3_3Box[1] < moveNumber && moveNumber <= Best3_3Box[2]){
                LCD_ShowString(56,8,"PRIZE!",0xffff);
                LCD_ShowString(16,32,"Your Score: ",0xffff);
                LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
                Best3_3Box[2]=moveNumber; 
            }
            else{
                LCD_ShowString(52,8,"FINISH!",0xffff);
                LCD_ShowString(16,32,"Your Score: ",0xffff);
                LCD_ShowNum(112,32,moveNumber,numlen(moveNumber),0xffff);
                LCD_ShowString(16,64,"Scoreboard: CTR>>",0xffff);
            }
            while(1){
                if(Get_Button(JOY_CTR)){
                    return 0;
                }
                delay(200);
            }
        }
    }
}