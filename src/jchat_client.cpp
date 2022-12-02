/*
 * TcpClient_test.cpp
 *
 * @build   make evpp
 * @server  bin/TcpServer_test 1234
 * @client  bin/TcpClient_test 1234
 *
 */

#include <iostream>

#include "TcpClient.h"
#include "htime.h"
#include "cJSON.h"
#include "ncurses.h"

#define TEST_RECONNECT  1
#define TEST_TLS        0

using namespace hv;

WINDOW *chat_main_win,*input_text_win,*chat_main_win_sub,*input_text_win_sub;
const char* input_head_style = "|/-\\";
int input_head_style_updateflag = 0;
int input_head_ptr = 0;

cJSON* create_json_message(char *inputbuf,char *username){
    cJSON* cjson_message = NULL;
    cjson_message = cJSON_CreateObject();
    cJSON_AddStringToObject(cjson_message,"content",inputbuf);
    cJSON_AddStringToObject(cjson_message,"sender",username);
    return cjson_message;
}

void parse_json_message(char *buff){
    cJSON* cjson_message = NULL;
    cJSON* cjson_name = NULL;
    cJSON* cjson_content = NULL;
    cjson_message = cJSON_Parse(buff);
    cjson_name = cJSON_GetObjectItem(cjson_message,"sender");
    cjson_content = cJSON_GetObjectItem(cjson_message,"content");
    // int x,y;
    // getyx(input_text_win_sub,y,x);
    //std::string output_str;
    //sprintf((char*)output_str.c_str(),"%s -> All : %s \n",cjson_name->valuestring,cjson_content->valuestring);
    wprintw(chat_main_win_sub,"%s -> All : %s \n",cjson_name->valuestring,cjson_content->valuestring);
    // move(LINES-1,x);
    //wadd_wchstr(chat_main_win_sub,output_str.c_str());
    //waddwstr(chat_main_win_sub,(wchar_t*)output_str.c_str());
    refresh();
    wrefresh(chat_main_win_sub);
    cJSON_Delete(cjson_message);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s ip username\n", argv[0]);
        return -10;
    }
    std::string ip = argv[1];
    std::string name = argv[2];
    //printf(" target ip %s \n",ip.data());
    // char *ip = argv[1];

    setlocale(LC_CTYPE,"en_US.UTF-8");
    initscr();
    cbreak();
    start_color();
    noecho();
    int maxx,maxy;
    init_pair(1,COLOR_RED,COLOR_BLACK);
    init_pair(2,COLOR_YELLOW,COLOR_BLACK);
    init_pair(3,COLOR_CYAN,COLOR_BLACK);
    init_pair(4, COLOR_GREEN,COLOR_BLACK);
    getmaxyx(stdscr,maxy,maxx);
    chat_main_win = newwin(maxy-3,maxx,0,0);
    // chat_main_win_sub = subwin(chat_main_win,maxy/2-2,maxx-2,1,1);
    input_text_win = newwin(3,maxx,maxy-3,0);
    chat_main_win_sub = subwin(stdscr,maxy-3-2,maxx-2,1,1);
    input_text_win_sub = subwin(stdscr,1,maxx-2,maxy-2,1);
    // if( chat_main_win==NULL || chat_main_win_sub==NULL || input_text_win==NULL || input_text_win_sub==NULL)
    // {
    //     endwin();
    //     puts("Some kind of error creating the windows");
    //     return(1);
    // }
    if( chat_main_win==NULL || input_text_win==NULL || chat_main_win_sub ==NULL ||input_text_win_sub==NULL)
    {
        endwin();
        puts("Some kind of error creating the windows");
        return(1);
    }
    box(chat_main_win,0,0);
    box(input_text_win,0,0);
    refresh();
    wrefresh(chat_main_win);
    wrefresh(input_text_win);

    TcpClient cli;
    int connfd = cli.createsocket(6666,ip.data());
    if (connfd < 0) {
        return -20;
    }
    // wprintw(chat_main_win_sub,"client connect to port %d, connfd=%d ...\n", 6666, connfd);
    // wrefresh(chat_main_win_sub);
    // wrefresh(chat_main_win);
    // wrefresh(input_text_win);
    cli.onConnection = [&cli](const SocketChannelPtr& channel) {
        std::string peeraddr = channel->peeraddr();
        if (channel->isConnected()) {
            // wprintw(chat_main_win_sub,"connected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
            // wrefresh(chat_main_win_sub);
            // send(time) every 3s
            setInterval(300, [channel](TimerID timerID){
                if (channel->isConnected()) {
                    if (channel->isWriteComplete()) {
                        // char str[DATETIME_FMT_BUFLEN] = {0};
                        // datetime_t dt = datetime_now();
                        // datetime_fmt(&dt, str);

                        //channel->write(str);
                    }
                } else {
                    killTimer(timerID);
                }
            });
            
        } else {
            wprintw(chat_main_win_sub,"disconnected to %s! connfd=%d\n", peeraddr.c_str(), channel->fd());
            wrefresh(chat_main_win_sub);
        }
        if (cli.isReconnect()) {
            wprintw(chat_main_win_sub,"reconnect cnt=%d, delay=%d\n", cli.reconn_setting->cur_retry_cnt, cli.reconn_setting->cur_delay);
            wrefresh(chat_main_win_sub);
        }
    };
    cli.onMessage = [](const SocketChannelPtr& channel, Buffer* buf) {
        //printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
		char *firstc;
		firstc = (char*)buf->data();
		if(firstc[0]=='{')
        	parse_json_message((char*)buf->data());
		else{
        	printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
		}
    };

    

#if TEST_RECONNECT
    // reconnect: 1,2,4,8,10,10,10...
    reconn_setting_t reconn;
    reconn_setting_init(&reconn);
    reconn.min_delay = 1000;
    reconn.max_delay = 10000;
    reconn.delay_policy = 2;
    cli.setReconnect(&reconn);
#endif

#if TEST_TLS
    cli.withTLS();
#endif

    cli.start();

    std::string str;
    int ch ;
    int running_flag=1;
    nodelay(stdscr,false);
    keypad(stdscr,TRUE);
    move(maxy-2,1);
    scrollok(chat_main_win_sub,TRUE);
    scrollok(input_text_win_sub,TRUE);
    while(running_flag){
        ch = getch();
        // if(ch == ERR){
            // if(input_head_style_updateflag){
                int x,y;
                getyx(input_text_win_sub,y,x);
                move(maxy-2,0);
                attron(COLOR_PAIR(input_head_ptr));
                addch(input_head_style[input_head_ptr++]);
                attroff(COLOR_PAIR(input_head_ptr));
                move(maxy-2,x+2);
                //wrefresh(input_text_win_sub);
                if(input_head_ptr >= 4) input_head_ptr =0;
                // input_head_style_updateflag = 0;
            // }
            // continue;
        // }
        //printf("%s\n",str.data());
        if(ch == '\n'){
            cJSON* cjson;
            std::string str_cjson;
            wmove(input_text_win_sub,0,0);
            wclrtoeol(input_text_win_sub);
            wrefresh(input_text_win_sub);
            str.push_back(ch);
            cjson = create_json_message((char*)str.data(),(char*)name.data());
            if (!cli.isConnected()) continue;
            str_cjson = cJSON_Print(cjson);
            cli.send(str_cjson);
            str.clear();
            cJSON_Delete(cjson);
            continue;
        }
        if((ch == KEY_BACKSPACE) && str.size()!=0){
            wmove(input_text_win_sub,getcury(input_text_win_sub),getcurx(input_text_win_sub) - 1);
            wdelch(input_text_win_sub);
            wrefresh(input_text_win_sub);
            str.pop_back();
        }else{
            if(ch == KEY_BACKSPACE) continue;
            waddch(input_text_win_sub,ch);
            wrefresh(input_text_win_sub);
            str.push_back(ch);
        }
        
        // if(ch != '\n')   {
        //     move(0,0);
        //     printw("%s",str.data());
        //     refresh();
        // }else{
        //     cli.stop();
        //     str.clear();
        //     break;
        // }
    }
    // while (std::getline(std::cin, str)) {
    //     if (str == "close") {
    //         cli.closesocket();
    //     } else if (str == "start") {
    //         cli.start();
    //     } else if (str == "stop") {
    //         cli.stop();
    //         break;
    //     } else {
    //         if (!cli.isConnected()) break;
    //         cJSON* cjson = create_json_message((char*)str.data(),(char*)name.data());
    //         str = cJSON_Print(cjson);
    //         cli.send(str);
    //         cJSON_Delete(cjson);
    //     }
    // }

    endwin();

    return 0;
}
