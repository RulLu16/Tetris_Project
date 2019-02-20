#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;
    int i,j;

    initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
        case MENU_RANK: rank(); break;
        case MENU_RECOMMEND: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break; 
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
    nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
    gameOver=0;
	timed_out=0;
    recRoot=malloc(sizeof(RecNode));
    recRoot->score=0;
    recRoot->lv=-1;
    for(i=0;i<HEIGHT;i++)
        for(j=0;j<WIDTH;j++)
            recRoot->recField[i][j]=field[i][j];
    create_tree(recRoot);
    recommend(recRoot);
    //modified_recommend(recRoot);
	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(10,WIDTH+10);
    DrawBox(10,WIDTH+10,4,8);
    move(17,WIDTH+10);
	printw("SCORE");
	DrawBox(18,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(19,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(11+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
    int i,j;
    int move_flag=0;
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            if(block[currentBlock][blockRotate][i][j]==1)
            {
                if(i+blockY>=HEIGHT || j+blockX<0 || j+blockX>=WIDTH)
                    move_flag=1;
                else if(f[i+blockY][j+blockX]==1)
                    move_flag=1;
            }
        }
    }
    if(move_flag==1)
        return 0;
    else
        return 1;
	
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
    int tempx, tempy;
    int tempr=0;
    int i,j;
    switch(command)
    {
        case KEY_UP :
            tempx=blockX;
            tempy=blockY;
            tempr=(blockRotate+3)%4;
            break;
        case KEY_DOWN :
            tempx=blockX;
            tempy=blockY-1;
            tempr=blockRotate;
            break;
        case KEY_RIGHT :
            tempx=blockX-1;
            tempy=blockY;
            tempr=blockRotate;
            break;
        case KEY_LEFT :
            tempx=blockX+1;
            tempy=blockY;
            tempr=blockRotate;
            break;
    }
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            if(block[currentBlock][tempr][i][j]==1)
            {
                move(i+tempy+1,j+tempx+1);
                printw(".");
            }
        }
    }
    while(1)
    {
        if(CheckToMove(f,currentBlock,tempr,tempy,tempx)==0)
            break;
        else
            tempy++;
    }
    tempy--;
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            if(block[currentBlock][tempr][i][j]==1)
            {
                move(i+tempy+1,j+tempx+1);
                printw(".");
            }
        }
    }
    DrawBlockWithFeatures(blockY,blockX,currentBlock,blockRotate);
    move(100,100);
}

void BlockDown(int sig){
    int i,j;
    if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)==1)
    {
        blockY++;
        timed_out=0;
        DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
        timed_out=0;
    }
    else
    {
        if(blockY==-1)
            gameOver=1;
        else 
        {
            score=score+AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX)+DeleteLine(field);
            nextBlock[0]=nextBlock[1];
            nextBlock[1]=nextBlock[2];
            nextBlock[2]=rand()%7;
            blockRotate=0;
            blockY=-1;
            blockX=WIDTH/2-2;
            DrawNextBlock(nextBlock);
            PrintScore(score);
            DrawField();
            recRoot->score=0;
            recommendY=-1;
            recommendX=blockX;
            recommendR=0;
            recRoot->lv=0;
            for(i=0;i<HEIGHT;i++)
                for(j=0;j<WIDTH;j++)
                    recRoot->recField[i][j]=field[i][j];
            recommend(recRoot);
            //modified_recommend(recRoot);
            timed_out=0;
        }

    }

}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
    int i,j;
    int touched=0;
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            /*if(block[currentBlock][blockRotate][i][j]==1)
                f[blockY+i][blockX+j]=1;
            if(block[currentBlock][blockRotate][i][j]==1 && blockY+i==HEIGHT-1)
                touched++;*/
            if(block[currentBlock][blockRotate][i][j]==1)
            {
                f[blockY+i][blockX+j]=1;
                if(blockY+i==HEIGHT-1)
                    touched++;
                else if(f[blockY+i+1][blockX+j]==1)
                    touched++;
                //if(blockX+j==WIDTH-1 || blockX+j==0)
                  //  touched++;
            }
        }
    }
    DrawField();
    return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i,j,k;
    int del[22]={0,};
    int dflag=0;
    int flag=0;
    int temp_score=0;
    for(i=0;i<HEIGHT;i++)
    {
        flag=0;
        for(j=0;j<WIDTH;j++)
        {
            if(f[i][j]==0)
                flag=1;
        }
        if(flag==0)
        {
            del[dflag]=i;
            dflag++;
        }
    }
    for(i=0;i<dflag;i++)
    {
        for(j=del[i];j>-1;j--)
        {
            for(k=0;k<WIDTH;k++)
            {
                f[j][k]=0;
                f[j][k]=f[j-1][k];
            }
        }
    }
    temp_score=dflag*dflag*100;
    return temp_score;

}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	while(1)
    {
        if(CheckToMove(field,blockID,blockRotate,y,x)==0)
            break;
        else
            y++;
    }
    y--;
    DrawBlock(y,x,blockID,blockRotate,'/');
}

void createRankList(){
    int num,i;
    head=malloc(sizeof(Node));
    Node* temp=head;
	FILE* fp=fopen("rank.txt","r");
    fscanf(fp,"%d",&num);
    for(i=0;i<num;i++)
    {
        fscanf(fp,"%s %d",temp->name,&temp->score);
        temp->link=malloc(sizeof(Node));
        temp=temp->link;
    }
    temp=NULL;
    fclose(fp);
}

void rank(){
    clear();
    printw("1. list ranks from X to Y\n");
    printw("2. list ranks by a specific name\n");
    printw("3. delete a specific rank\n");

    switch(wgetch(stdscr))
    {
        case '1':
            rmenu_one();
            break;
        case '2':
            rmenu_two();
            break;
        case '3':
            rmenu_three();
            break;
        default: break;
    }
    getch();

}

void rmenu_one()
{
    FILE* fp=fopen("rank.txt","r");
    int max;
    fscanf(fp,"%d",&max);
    fclose(fp);
	int x,y;
    char inx,iny;
    int count;
    createRankList();
    printw("X : ");
    echo();
    scanw("%c",&inx);
    printw("Y : ");
    scanw("%c",&iny);
    noecho();
    printw("\tname\t| score  \n");
    printw("--------------------------\n");
    Node* temp=head;
    if(inx==0)
        x=1;
    else
        x=(int)(inx-48);
    if(iny==0)
        y=max;
    else
        y=(int)(iny-48);
    if(x<1 || y<x)
    {
        printw("search failure : no rank in the list\n");
        getch();
        return;
    }
            
    count=0;
    while(1)
    {   
        if(temp->link==NULL)
            break;
        count++;
        if(count>=x && count<=y)
        {
            printw("%s\t\t| %d\n",temp->name,temp->score);
            temp=temp->link;
        }
        else if(count>y)
            break;
        else
            temp=temp->link;
    }

}

void rmenu_two()
{
    createRankList();
    char name[NAMELEN]; 
    int flag=0;
    printw("Input the name : ");
    echo();
    getstr(name);
    noecho();
    Node* temp=head;
    printw("\tname\t| score  \n");
    printw("--------------------------\n");

    while(1)
    {
        if(temp->link==NULL)
            break;
        else
        {
            if(strcmp(temp->name,name)==0)
            {
                printw("%s\t\t| %d\n",temp->name,temp->score);
                temp=temp->link;
                flag++;
            }
            else
                temp=temp->link;
        }
    }
    if(flag==0)
        printw("search failure : no name in the list\n");

}

void rmenu_three()
{
    FILE* fp=fopen("rank.txt","r");
    int max;
    fscanf(fp,"%d",&max);
    fclose(fp);
    createRankList();
    int num;
    int count=1;
    Node* temp=head->link;
    Node* pre=head;
    printw("Input the rank : ");
    echo();
    scanw("%d",&num);
    noecho();
    if(num<=0 || num>max)
    {
        printw("search failure : the rank is not in the list\n");
        return;
    }
    if(num==1)
    {
        head=temp;
        free(pre);
        printw("result : the rank deleted\n");
        writeRankFile();
        return;
    }
    while(1)
    {
        count++;
        if(temp->link==NULL)
            break;
        else
        {
            if(count==num)
            {
                pre->link=temp->link;
                free(temp);
                printw("result : the rank deleted\n");
                break;
            }
            else
            {
                pre=pre->link;
                temp=temp->link;
            }
        }     
    }
    writeRankFile();
    
}
void writeRankFile(){
    FILE* fp=fopen("rank.txt","w");
    int i=0;
    int j;
    Node* temp=head;
    while(1)
    {
        if(temp->link==NULL)
            break;
        else{
            i++;
            temp=temp->link;
        }
    }
    fprintf(fp,"%d\n",i);
    temp=head;
    for(j=0;j<i;j++)
    {
            fprintf(fp,"%s %d\n",temp->name,temp->score);
            temp=temp->link;
     
    }
    fclose(fp);
}

void newRank(int score){
	char name[NAMELEN];
    createRankList();
    clear();
    printw("Your name : ");
    echo();
    getstr(name);
    noecho();
    printw(name);
    Node* pre=head;
    Node* cur=head->link;
    Node* temp=malloc(sizeof(Node));
    strcpy(temp->name,name);
    temp->score=score;
    if(pre->score <= temp->score)
    {
        temp->link=pre;
        head=temp;
        writeRankFile();
        return;
    }
    while(1)
    {
        if(cur==NULL)
        {
            temp->link=cur;
            pre->link=temp;
            break;
        }
        else
        {
            if(cur->score <= temp->score)
            {
                temp->link=cur;
                pre->link=temp;
                break;
            }
            else
            {
                pre=pre->link;
                cur=cur->link;
            }
        }
    }
    writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
    DrawBlock(y,x,blockID,blockRotate,'R');
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
    int index=0;
    int r,x,y;
    int h,w;
    int tmax=0;
    RecNode** c=root->child;
    if(root->lv>BLOCK_NUM-1) return 0;
    for(r=0;r<4;r++)
    {
        for(x=-3;x<WIDTH;x++)
        {
            y=blockY;
            if(CheckToMove(root->recField,nextBlock[root->lv],r,y,x))
            {
            while(CheckToMove(root->recField,nextBlock[root->lv],r,y+1,x)) y++; 
            for(h=0;h<HEIGHT;h++)
                for(w=0;w<WIDTH;w++)
                    c[index]->recField[h][w]=root->recField[h][w];
            c[index]->curBlockID=nextBlock[root->lv];
            c[index]->recBlockX=x;
            c[index]->recBlockY=y;
            c[index]->recBlockRotate=r;
            c[index]->score=root->score + AddBlockToField(c[index]->recField,nextBlock[root->lv], r,y,x);
            c[index]->score=c[index]->score+DeleteLine(c[index]->recField);
            if(root->lv==BLOCK_NUM-2)
                max=c[index]->score;
            else{
                tmax=recommend(c[index]);
                if(max<tmax)
                {
                    max=tmax;
                    if(root->lv==0)
                    {
                        recommendY=y;
                        recommendX=x;
                        recommendR=r;
                    }
                }
                else if(max==tmax)
                {
                    if(recommendY<y)
                    {
                        max=tmax;
                        if(root->lv==0)
                        {
                            recommendY=y;
                            recommendX=x;
                            recommendR=r;
                        }
                    }
                }
            }
            index++;
            }
        }
    }

	return max;
}

void recommendedPlay(){
    int command;
    clear();
    act.sa_handler=recBlockDown;
    sigaction(SIGALRM,&act,&oact);
    recInitTetris();
    do{
        if(timed_out==0)
        {
            alarm(1);
            timed_out=1;
        }
        command=wgetch(stdscr);
        switch(command)
        {
            case 'q': command=QUIT;
                      break;
            case 'Q': command=QUIT;
                      break;
            default : command=NOTHING;
                      break;
        }
        if(command==QUIT)
        {
            alarm(0);
            DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
            move(HEIGHT/2,WIDTH/2-4);
            printw("Good-bye!!");
            refresh();
            getch();

            return;
        }
    }while(!gameOver);

    alarm(0);
    getch();
    DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
    move(HEIGHT/2,WIDTH/2-4);
    printw("GameOver!!");
    refresh();
    getch();	
}

void recInitTetris()
{
    int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
    nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
    gameOver=0;
	timed_out=0;
    recRoot=malloc(sizeof(RecNode));
    recRoot->score=0;
    recRoot->lv=-1;
    for(i=0;i<HEIGHT;i++)
        for(j=0;j<WIDTH;j++)
            recRoot->recField[i][j]=field[i][j];
    create_tree(recRoot);
    recommend(recRoot);
    //modified_recommend(recRoot);
	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
    DrawRecommend(recommendY,recommendX,nextBlock[0],recommendR);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void recBlockDown(int sig)
{
    int i,j;
        if(recommendY<0)
            gameOver=1;
        else 
        {
            score=score+AddBlockToField(field,nextBlock[0],recommendR,recommendY,recommendX)+DeleteLine(field);
            nextBlock[0]=nextBlock[1];
            nextBlock[1]=nextBlock[2];
            nextBlock[2]=rand()%7;
            blockRotate=0;
            blockY=-1;
            blockX=WIDTH/2-2;
            DrawNextBlock(nextBlock);
            PrintScore(score);
            DrawField();
            recRoot->score=0;
            recommendY=-1;
            recommendX=blockX;
            recommendR=0;
            recRoot->lv=0;
            for(i=0;i<HEIGHT;i++)
                for(j=0;j<WIDTH;j++)
                    recRoot->recField[i][j]=field[i][j];
            recommend(recRoot);
            //modified_recommend(recRoot);
            DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
            DrawRecommend(recommendY,recommendX,nextBlock[0],recommendR);
            timed_out=0;
        }        
}
void modified_recommend(RecNode* root){
    int i,j;
    int y;
    int h,w;
    int max=0;
    int index=0;
    int choice;
    srand(time(NULL));
    mod_rec m[CHILDREN_MAX];
    RecNode** m_c=root->child;
    for(i=0;i<CHILDREN_MAX;i++)
       m_c[i]=malloc(sizeof(RecNode));
    for(i=0;i<4;i++)
    {
        for(j=-3;j<WIDTH;j++)
        {
            y=blockY;
            if(CheckToMove(root->recField,nextBlock[0],i,y,j))
            {
                while(CheckToMove(root->recField,nextBlock[0],i,y+1,j)) y++;
                for(h=0;h<HEIGHT;h++)
                    for(w=0;w<WIDTH;w++)
                        m_c[index]->recField[h][w]=root->recField[h][w];
                m_c[index]->recBlockX=j;
                m_c[index]->recBlockY=y;
                m_c[index]->recBlockRotate=i;
                m_c[index]->score=root->score+AddBlockToField(m_c[index]->recField,nextBlock[0],i,y,j);
                m_c[index]->score=m_c[index]->score+DeleteLine(m_c[index]->recField);
                if(max<m_c[index]->score)
                    max=m_c[index]->score;
                index++;
            }
        }

    }
    index=0;
    for(i=0;i<CHILDREN_MAX;i++)
    {
        if(m_c[i]->score==max)
        {
            m[index].x=m_c[i]->recBlockX;
            m[index].y=m_c[i]->recBlockY;
            m[index].r=m_c[i]->recBlockRotate;
            index++;
        }
    }
    choice=(int)rand()%index;
    recommendY=m[choice].y;
    recommendX=m[choice].x;
    recommendR=m[choice].r;

}

void create_tree(RecNode* root)
{
    int i;
    RecNode** c=root->child;
    for(i=0;i<CHILDREN_MAX;i++)
    {
        c[i]=(RecNode*)malloc(sizeof(RecNode));
        c[i]->lv=root->lv+1;
        c[i]->parent=root;
    	if(c[i]->lv<BLOCK_NUM-1)
        	create_tree(c[i]);
    }
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
    DrawBlock(y,x,blockID,blockRotate,' ');
    DrawRecommend(recommendY,recommendX,blockID,recommendR);
    DrawShadow(y,x,blockID,blockRotate);
}
