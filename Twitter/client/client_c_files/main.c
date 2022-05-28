//include:
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "slre.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
//define:
#define MAX_SIZE 100000
#define MAX_SIZE2 1000
//prototype:
int initialize (int);
char *send_data(char *);
void (*welcome_page())();
void (*Signup_page())();
void (*Login_page())();
void (*Menu_page())();
void (*Personal_area_page())();
void (*Set_Bio_page())();
void (*Change_Password_page())();
void (*Timeline_page())();
void (*Send_tweet_page())();
void (*Search_page())();
void (*Unfollow_page())();
void (*Follow_page())();
void (*Refresh_page())();
void (*Tweet_profile_page())();
void (*Like_Comment_page())();
void (*Like_page())();
void (*Comment_page())();
void (*See_comments_page())();
void (*Goodbye_page()) ();
void say_welcome_page();
void Goodbye_page2();
int Signup_mes_analise(char *);
int Login_mes_analise(char *);
int Set_Bio_mes_analise(char *);
int Change_Password_mes_analise(char *);
int Send_tweet_mes_analise(char *);
int Search_mes_analise(char *);
void Show_Search_results_tweets(char *);
int Follow_or_Unfollow_mes_analise(char *);
int Show_Search_results(char *);
int Refresh_mes_analise(char *);
int Tweet_profile_mes_analise(char *);
void Show_Tweet_profile_results(char *);
void Show_Tweet_profile_results_tweets(char *);
void Like_Comment_mes_analise(char *);
int Goodbye_mes_analise(char *);
bool space_error(char * input);
//global:
struct sockaddr_in server;
int client_index=0;
char Token[100];
char Follow_or_Unfollow_Username[MAX_SIZE];
char tweets[MAX_SIZE2][MAX_SIZE2];
char client_username [MAX_SIZE];
//end
int main() {
    int port=12345;
    void *(*call_function)();
    int can_initialize= initialize(port);
    call_function=&welcome_page;
    say_welcome_page();
    while( can_initialize==0 && call_function!=&Goodbye_page2) {
        call_function=call_function();
    }
    Goodbye_page2();
    return 0;
}
int initialize (int port){
    WSADATA wsadata;
    int wsaStartUp= WSAStartup( MAKEWORD (2,2),&wsadata);
    if(wsaStartUp!=0){ //Error
        printf("Error");
        return -1;
    }
    server.sin_family= AF_INET;
    server.sin_addr.s_addr= inet_addr("127.0.0.1");
    server.sin_port= htons(port);
    return 0; //return 0 if ok
}
char *send_data(char *data){
    char *buffer= malloc(MAX_SIZE);
    memset(buffer,0,MAX_SIZE);
    int client_socket= socket(AF_INET,SOCK_STREAM,0);
    if (client_socket == INVALID_SOCKET){ //Error=INVALID_SOCKET
        return buffer;
    }
    int can_connect= connect(client_socket, (struct sockaddr *) &server, sizeof(server) );
    if (can_connect != 0){ //return 0 if ok
        return buffer;
    }
    send(client_socket, data, strlen(data), 0);
    recv(client_socket, buffer, MAX_SIZE-1, 0);
    closesocket(client_socket);
    return buffer;
}
void (*welcome_page()) (){
    static int check=0;
    printf("----------\n");
    printf("1.Login\n2.Signup\n");
    char request[MAX_SIZE];
    //scanf("%d",&request);
    if(check==0){
        scanf("%[^\n]s",&request);
        check++;
    }
    else{
        scanf("%*c%[^\n]s",&request);
        check++;
    }
    if (!strcmp(request,"1")){
        return Login_page();
    }
    else if (!strcmp(request,"2")){
        return Signup_page;
    }
    else{
        printf("Invalid request\nTry again\n");
        return welcome_page;
    }
}
void (*Signup_page())(){
    printf("----------\n");
    printf("Username:\n");
    char Username[MAX_SIZE];
    scanf("%*c%[^\n]s",&Username);
    printf("Password:\n");
    char Password[MAX_SIZE];
    scanf("%*c%[^\n]s",&Password);
    if(space_error(Password)==true || space_error(Username)==true){
        printf("You can not use space in your username or password\n");
        return welcome_page;
    }
    char message[MAX_SIZE]="signup ";
    strcat(message, Username);
    strcat(message, ", ");
    strcat(message, Password);
    strcat(message, "\n");
    char *feedback=send_data(message);
    int success=Signup_mes_analise(feedback);
    if(success==0){//0 means Error
        return welcome_page;
    }
    if (success==1){//1 means Successful
        printf("Now you can login with your account\n");
        return welcome_page;
    }
}
void (*Login_page())(){
    memset(client_username,0,MAX_SIZE);
    printf("----------\n");
    printf("Username:\n");
    char Username[MAX_SIZE];
    scanf("%*c%[^\n]s",&Username);
    strcpy(client_username,Username);
    printf("Password:\n");
    char Password[MAX_SIZE];
    scanf("%*c%[^\n]s",&Password);
    if(space_error(Password)==true || space_error(Username)==true){
        printf("You can not use space in your username or password\n");
        return welcome_page;
    }
    char message[MAX_SIZE]="login ";
    strcat(message, Username);
    strcat(message, ", ");
    strcat(message, Password);
    strcat(message, "\n");
    char *feedback=send_data(message);
    int Success=Login_mes_analise(feedback);
    if(Success==0){//0 means Error
        printf("Please try again\n");
        return welcome_page;
    }
    if (Success==1){//1 means Successful
        return Menu_page;
    }
}
int Signup_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":\"([^\n]+)\"";
    int num_caps=2;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return 0;
    }
    else if(!strcmp(caps[0],"Successful")){
        printf("%s\n",caps[1]);
        return 1;
    }
}
int Login_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":\"([^\n]+)\"";
    int num_caps=2;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return 0;
    }
    else if(!strcmp(caps[0],"Token")){
        strcpy(Token,caps[1]);
        printf("Successful you have loged in.\n");
        return 1;
    }
}
void (*Menu_page())(){
    printf("----------\n");
    printf("1.Timeline\n2.Search\n3.Tweet Profile\n4.Personal area\n5.Log out\n");
    int input;
    char request[MAX_SIZE];
    scanf("%*c%[^\n]s",&request);
    switch (request[0]) {
        case '1':
            return Timeline_page;
            break;
        case '2':
            return Search_page;
            break;
        case '3':
            return Tweet_profile_page;
            break;
        case '4':
            return Personal_area_page;
            break;
        case '5':
            return Goodbye_page;
            break;
        default:
            printf("Invalid request\nTry again\n");
            return Menu_page;
            break;
    }
}
void (*Personal_area_page())(){
    printf("----------\n");
    printf("1.Set Bio\n2.Change Password\n3.Back\n");
    char request[MAX_SIZE];
    scanf("%*c%[^\n]s",&request);
    switch (request[0]) {
        case '1':
            return Set_Bio_page;
            break;
        case '2':
            return Change_Password_page;
            break;
        case '3':
            return Menu_page;
            break;
        default:
            printf("Invalid request\nTry again\n");
            return Personal_area_page;
            break;
    }
}
void (*Set_Bio_page())(){
    printf("----------\n");
    printf("Write your Bio\n");
    char Bio[MAX_SIZE];
    scanf("%*c%[^\n]s",&Bio);
    char message[MAX_SIZE]="set bio ";
    strcat(message, Token);
    strcat(message, ", ");
    strcat(message, Bio);
    strcat(message, "\n");
    char *feedback=send_data(message);
    int Success=Set_Bio_mes_analise(feedback);
    if(Success==0){//0 means Error
        return Personal_area_page;
    }
    if (Success==1){//1 means Successful
        return Personal_area_page;
    }
}
int Set_Bio_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":\"([^\n]+)\"";
    int num_caps=2;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Successful")){
        printf("%s\n",caps[1]);
        return 1;
    }
    else if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return 0;
    }
}
void (*Change_Password_page())(){
    printf("----------\n");
    printf("Currect Password:\n");
    char Currect[MAX_SIZE];
    scanf("%*c%[^\n]s",&Currect);
    printf("New Password:\n");
    char New[MAX_SIZE];
    scanf("%*c%[^\n]s",&New);
    if(space_error(Currect)==true || space_error(New)==true){
        printf("You can not use space in your password\n");
        return Personal_area_page;
    }
    char message[MAX_SIZE]="change password ";
    strcat(message, Token);
    strcat(message, ", ");
    strcat(message, Currect);
    strcat(message, ", ");
    strcat(message, New);
    strcat(message, "\n");
    char *feedback=send_data(message);
    int Success=Change_Password_mes_analise(feedback);
    if(Success==0){//0 means Error
        return Personal_area_page;
    }
    if (Success==1){//1 means Successful
        return Personal_area_page;
    }
}
int Change_Password_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":\"([^\n]+)\"";
    int num_caps=2;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Successful")){
        printf("%s\n",caps[1]);
        return 1;
    }
    else if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return 0;
    }
}
void (*Timeline_page())(){
    printf("----------\n");
    printf("1.Refresh\n2.Send Tweet\n3.Back\n");
    char request[MAX_SIZE];
    scanf("%*c%[^\n]s",&request);
    switch (request[0]) {
        case '1':
            return Refresh_page;
            break;
        case '2':
            return Send_tweet_page;
            break;
        case '3':
            return Menu_page;
            break;
        default:
            printf("Invalid request\nTry again\n");
            return Timeline_page;
            break;
    }
}
void (*Send_tweet_page())(){
    printf("----------\n");
    printf("Write your tweet:\n");
    char tweet[MAX_SIZE];
    scanf("%*c%[^\n]s",tweet);
    char message[MAX_SIZE]="send tweet ";
    strcat(message, Token);
    strcat(message, ", ");
    strcat(message, tweet);
    strcat(message, "\n");
    char *feedback=send_data(message);
    int Success=Send_tweet_mes_analise(feedback);
    if(Success==0){//0 means Error
        printf("Please try again\n");
        return Timeline_page;
    }
    if (Success==1){//1 means Successful
        return Timeline_page;
    }
    return Timeline_page;
}
int Send_tweet_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":\"([^\n]+)\"";
    int num_caps=2;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return 0;
    }
    else if(!strcmp(caps[0],"Successful")){
        printf("%s\n",caps[1]);
        return 1;
    }
}
void (*Goodbye_page()) (){
    char message[MAX_SIZE]="logout ";
    strcat(message, Token);
    strcat(message, "\n");
    char *feedback=send_data(message);
    int Success=Goodbye_mes_analise(feedback);
    if(Success==1) {
        return Goodbye_page2;
    }
    else{
        return Menu_page;
    }
}
void Goodbye_page2(){
    printf("Goodbye");
}
int Goodbye_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":\"([^\n]+)\"";
    int num_caps=2;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return 0;
    }
    else if(!strcmp(caps[0],"Successful")){
        printf("%s\n",caps[1]);
        return 1;
    }
}
void (*Search_page())(){
    printf("----------\n");
    printf("Write Username:\n");
    char Username[MAX_SIZE];
    scanf("%*c%[^\n]s",Username);
    if(space_error(Username)==true){
        printf("You can not use space in username\n");
        return Menu_page;
    }
    if(!strcmp(client_username,Username)){
        printf("You can not search your username.\n");
        return Menu_page;
    }
    char message[MAX_SIZE]="search ";
    strcat(message, Token);
    strcat(message, ", ");
    strcat(message, Username);
    strcat(message, "\n");
    char *feedback=send_data(message);
    printf("----------\n");
    int Success=Search_mes_analise(feedback);
    if(Success==0){//0 means Error
        return Menu_page;
    }
    if (Success==11 ||Success==10){//1 means Successful
        if(Success==11){// 1 meanes followed
            return Unfollow_page();
        }
        else if(Success==10){
            return Follow_page();
        }
    }
}
int Search_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":\"([^\n]+)\"";
    int num_caps=2;
    int ans=0;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return 0;
    }
    else if(!strcmp(caps[0],"Profile")){
        ans=Show_Search_results(input);
        if(ans==1){
            return 11;
        }
        else if(ans==0) {
            return 10;
        }
    }
}
int Show_Search_results(char *input){
    char *tokenPtr1;
    char *tokenPtr2;
    char *tokenPtr3;
    char *tokenPtr4;
    char *tokenPtr5;
    char *tokenPtr6;
    char *tokenPtr7;
    tokenPtr1=strstr(input,"\"username\":\"");
    char tweets[6][MAX_SIZE];
    memset(tweets,0,6*MAX_SIZE);
    tokenPtr2=strstr(tokenPtr1+12,"\",\"bio\":\"");
    strncat(tweets[0],tokenPtr1+12,tokenPtr2-tokenPtr1-12);
    tokenPtr3=strstr(tokenPtr2+9,"\",\"numberOfFollowers\":");
    strncat(tweets[1],tokenPtr2+9,tokenPtr3-tokenPtr2-9);
    tokenPtr4=strstr(tokenPtr3+22,",\"numberOfFollowings\":");
    strncat(tweets[2],tokenPtr3+22,tokenPtr4-tokenPtr3-22);
    tokenPtr5=strstr(tokenPtr4+22,",\"followStatus\":\"");
    strncat(tweets[3],tokenPtr4+22,tokenPtr5-tokenPtr4-22);
    tokenPtr6=strstr(tokenPtr5+17,"\",\"allTweets\":[");
    strncat(tweets[4],tokenPtr5+17,tokenPtr6-tokenPtr5-17);
    tokenPtr7=strstr(tokenPtr5+15,"]}}");
    strncat(tweets[5],tokenPtr6+15,tokenPtr7-tokenPtr6-15);
    printf("Username: %s\n",tweets[0]);
    printf("Bio: %s\n",tweets[1]);
    printf("Followers: %s\n",tweets[2]);
    printf("Followings: %s\n",tweets[3]);
    printf("Follow status: %s\n",tweets[4]);
    strcpy(Follow_or_Unfollow_Username,tweets[0]);
    char *input2;
    input2=&tweets[5];
    Show_Search_results_tweets(input2);
    if(!strcmp(tweets[4],"Followed")) {
        return 1;
    }
    if(!strcmp(tweets[4],"NotFollowed")){
        return 0;
    }
}
void Show_Search_results_tweets(char *input){
    char *tokenPtr1;
    char *tokenPtr2;
    char *tokenPtr3;
    char *tokenPtr4;
    char *tokenPtr5;
    char *tokenPtr6;
    tokenPtr1=strstr(input,"\"id\":");
    int i=0;
    char tweets[MAX_SIZE2][MAX_SIZE2];
    memset(tweets,0,MAX_SIZE2*MAX_SIZE2);
    while( tokenPtr1!=NULL){
        tokenPtr2=strstr(tokenPtr1+5,",\"author\":\"");
        strncat(tweets[0+6*i],tokenPtr1+5,tokenPtr2-tokenPtr1-5);
        tokenPtr3=strstr(tokenPtr2+11,"\",\"content\":\"");
        strncat(tweets[1+6*i],tokenPtr2+11,tokenPtr3-tokenPtr2-11);
        tokenPtr4=strstr(tokenPtr3+13,"\",\"comments\":{");
        strncat(tweets[2+6*i],tokenPtr3+13,tokenPtr4-tokenPtr3-13);
        tokenPtr5=strstr(tokenPtr4+14,"},\"likes\":");
        strncat(tweets[3+6*i],tokenPtr4+14,tokenPtr5-tokenPtr4-14);
        tokenPtr6=strstr(tokenPtr5+10,"}");
        strncat(tweets[4+6*i],tokenPtr5+10,tokenPtr6-tokenPtr5-10);
        tokenPtr1=strstr(tokenPtr6+1,"\"id\":");
        i++;
    }
    char *tokenPtr;
    int counter;
    int j;
    for(j=0;j<i;j++) {
        counter=0;
        tokenPtr = strstr(tweets[3 + 6 * j], "\":\"");
        while (tokenPtr != NULL) {
            tokenPtr = strstr(tokenPtr+3, "\":\"");
            counter++;
        }
        snprintf( tweets[5+6*j], MAX_SIZE2, "%d", counter );
    }
    for(j=0;j<i;j++) {
        printf("\n");
        printf("%s         %s\n", tweets[1 + 6 * j], tweets[0 + 6 * j]);
        printf("%s\n", tweets[2 + 6 * j]);
        printf("Comments:%s         Likes:%s\n", tweets[5 + 6 * j], tweets[4 + 6 * j]);
    }
}
void (*Follow_page())(){
    printf("----------\n");
    printf("1.Follow\n2.Back\n");
    char message[MAX_SIZE]="follow ";
    char request[MAX_SIZE];
    scanf("%*c%[^\n]s",&request);
    switch (request[0]) {
        case '1':
            strcat(message, Token);
            strcat(message, ", ");
            strcat(message, Follow_or_Unfollow_Username);
            strcat(message, "\n");
            char *feedback=send_data(message);
            int Success=Follow_or_Unfollow_mes_analise(feedback);
            if(Success==0){//0 means Error
                printf("Please try again\n");
                return Follow_page;
            }
            if (Success==1){//1 means Successful
                return Unfollow_page;
            }
            break;
        case '2':
            return Menu_page;
            break;
        default:
            printf("Invalid request\nTry again\n");
            return Follow_page;
            break;
    }
}
void (*Unfollow_page())(){
    printf("----------\n");
    printf("1.Unfollow\n2.Back\n");
    char message[MAX_SIZE]="unfollow ";
    char request[MAX_SIZE];
    scanf("%*c%[^\n]s",&request);
    switch (request[0]) {
        case '1':
            strcat(message, Token);
            strcat(message, ", ");
            strcat(message, Follow_or_Unfollow_Username);
            strcat(message, "\n");
            char *feedback=send_data(message);
            int Success=Follow_or_Unfollow_mes_analise(feedback);
            if(Success==0){//0 means Error
                printf("Please try again\n");
                return Unfollow_page;
            }
            if (Success==1){//1 means Successful
                return Follow_page;
            }
            break;
        case '2':
            return Menu_page;
            break;
        default:
            printf("Invalid request\nTry again\n");
            return Unfollow_page;
            break;
    }
}
int Follow_or_Unfollow_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":\"([^\n]+)\"";
    int num_caps=2;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return 0;
    }
    else if(!strcmp(caps[0],"Successful")){
        printf("%s\n",caps[1]);
        return 1;
    }
}
void (*Refresh_page())(){
    printf("----------\n");
    char message[MAX_SIZE]="refresh ";
    strcat(message, Token);
    strcat(message, "\n");
    char*Refresh_feedback=send_data(message);
    char regex[]="\"type\":\"([^\n]+)\",\"message\":([^\n]+)";
    int num_caps=2;
    int ans;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, Refresh_feedback, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return Timeline_page;
    }
    else if(!strcmp(caps[0],"List")){
        ans=Refresh_mes_analise(Refresh_feedback);
        if(ans==0){
            return Menu_page;
        }
        else{
            return Like_Comment_page;
        }
    }
}
int Refresh_mes_analise(char *input){
    char *tokenPtr1;
    char *tokenPtr2;
    char *tokenPtr3;
    char *tokenPtr4;
    char *tokenPtr5;
    char *tokenPtr6;
    tokenPtr1=strstr(input,"\"id\":");
    int i=0;
    memset(tweets,0,MAX_SIZE2*MAX_SIZE2);
    while( tokenPtr1!=NULL){
        tokenPtr2=strstr(tokenPtr1+5,",\"author\":\"");
        strncat(tweets[0+6*i],tokenPtr1+5,tokenPtr2-tokenPtr1-5);
        tokenPtr3=strstr(tokenPtr2+11,"\",\"content\":\"");
        strncat(tweets[1+6*i],tokenPtr2+11,tokenPtr3-tokenPtr2-11);
        tokenPtr4=strstr(tokenPtr3+13,"\",\"comments\":{");
        strncat(tweets[2+6*i],tokenPtr3+13,tokenPtr4-tokenPtr3-13);
        tokenPtr5=strstr(tokenPtr4+14,",\"likes\":");
        strncat(tweets[3+6*i],tokenPtr4+14,tokenPtr5-tokenPtr4-14);
        tokenPtr6=strstr(tokenPtr5+9,"}");
        strncat(tweets[4+6*i],tokenPtr5+9,tokenPtr6-tokenPtr5-9);
        tokenPtr1=strstr(tokenPtr6+1,"\"id\":");
        i++;
    }
    char *tokenPtr;
    int counter;
    int j;
    for(j=0;j<i;j++) {
        counter=0;
        tokenPtr = strstr(tweets[3 + 6 * j], "\":\"");
        while (tokenPtr != NULL) {
            tokenPtr = strstr(tokenPtr+3, "\":\"");
            counter++;
        }
        snprintf( tweets[5+6*j], MAX_SIZE2, "%d", counter );
    }
    for(j=0;j<i;j++) {
        if(j!=0) {
            printf("\n");
        }
        printf("%s         %s\n", tweets[1 + 6 * j], tweets[0 + 6 * j]);
        printf("%s\n", tweets[2 + 6 * j]);
        printf("Comments:%s         Likes:%s\n", tweets[5 + 6 * j], tweets[4 + 6 * j]);
    }
    if(i==0){
        printf("Nothing to show\n");
        return 0;
    }
    return 1;
}
void (*Tweet_profile_page())(){
    printf("----------\n");
    char message[MAX_SIZE]="profile ";
    strcat(message, Token);
    strcat(message, "\n");
    char *feedback=send_data(message);
    int Success=Tweet_profile_mes_analise(feedback);
    if(Success==0){//0 means Error
        printf("Please try again\n");
        return Menu_page;
    }
    if (Success==1){
        Show_Tweet_profile_results(feedback);
        return Menu_page;
    }
}
int Tweet_profile_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":([^\n]+)";
    int num_caps=2;
    int ans=0;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        return 0;
    }
    else if(!strcmp(caps[0],"Profile")){
        return 1;
    }
}
void Show_Tweet_profile_results(char *input){
    char *tokenPtr1;
    char *tokenPtr2;
    char *tokenPtr3;
    char *tokenPtr4;
    char *tokenPtr5;
    char *tokenPtr6;
    char *tokenPtr7;
    tokenPtr1=strstr(input,"\"username\":\"");
    char tweets[6][MAX_SIZE];
    memset(tweets,0,6*MAX_SIZE);
    tokenPtr2=strstr(tokenPtr1+12,"\",\"bio\":\"");
    strncat(tweets[0],tokenPtr1+12,tokenPtr2-tokenPtr1-12);
    tokenPtr3=strstr(tokenPtr2+9,"\",\"numberOfFollowers\":");
    strncat(tweets[1],tokenPtr2+9,tokenPtr3-tokenPtr2-9);
    tokenPtr4=strstr(tokenPtr3+22,",\"numberOfFollowings\":");
    strncat(tweets[2],tokenPtr3+22,tokenPtr4-tokenPtr3-22);
    tokenPtr5=strstr(tokenPtr4+22,",\"followStatus\":\"");
    tokenPtr6=strstr(tokenPtr5+17,"\",\"allTweets\":[");
    strncat(tweets[3],tokenPtr4+22,tokenPtr5-tokenPtr4-22);
    strncat(tweets[4],tokenPtr5+17,tokenPtr6-tokenPtr5-17);
    tokenPtr7=strstr(tokenPtr6+15,"]}}");
    strncat(tweets[5],tokenPtr6+15,tokenPtr7-tokenPtr6-15);
    printf("Username: %s\n",tweets[0]);
    printf("Bio: %s\n",tweets[1]);
    printf("Followers: %s\n",tweets[2]);
    printf("Followings: %s\n",tweets[3]);
    strcpy(Follow_or_Unfollow_Username,tweets[0]);
    char *input2;
    input2=&tweets[5];
    Show_Tweet_profile_results_tweets(input2);
}
void Show_Tweet_profile_results_tweets(char *input){
    char *tokenPtr1;
    char *tokenPtr2;
    char *tokenPtr3;
    char *tokenPtr4;
    char *tokenPtr5;
    char *tokenPtr6;
    tokenPtr1=strstr(input,"\"id\":");
    int i=0;
    char tweets[MAX_SIZE2][MAX_SIZE2];
    memset(tweets,0,MAX_SIZE2*MAX_SIZE2);
    while( tokenPtr1!=NULL){
        tokenPtr2=strstr(tokenPtr1+5,",\"author\":\"");
        strncat(tweets[0+6*i],tokenPtr1+5,tokenPtr2-tokenPtr1-5);
        tokenPtr3=strstr(tokenPtr2+11,"\",\"content\":\"");
        strncat(tweets[1+6*i],tokenPtr2+11,tokenPtr3-tokenPtr2-11);
        tokenPtr4=strstr(tokenPtr3+13,"\",\"comments\":{");
        strncat(tweets[2+6*i],tokenPtr3+13,tokenPtr4-tokenPtr3-13);
        tokenPtr5=strstr(tokenPtr4+14,",\"likes\":");
        strncat(tweets[3+6*i],tokenPtr4+14,tokenPtr5-tokenPtr4-14);
        tokenPtr6=strstr(tokenPtr5+9,"}");
        strncat(tweets[4+6*i],tokenPtr5+9,tokenPtr6-tokenPtr5-9);
        tokenPtr1=strstr(tokenPtr6+1,"\"id\":");
        i++;
    }
    char *tokenPtr;
    int counter;
    int j;
    for(j=0;j<i;j++) {
        counter=0;
        tokenPtr = strstr(tweets[3 + 6 * j], "\":\"");
        while (tokenPtr != NULL) {
            tokenPtr = strstr(tokenPtr+3, "\":\"");
            counter++;
        }
        snprintf( tweets[5+6*j], MAX_SIZE2, "%d", counter );
    }
    for(j=0;j<i;j++) {
        printf("\n");
        printf("%s         %s\n", tweets[1 + 6 * j], tweets[0 + 6 * j]);
        printf("%s\n", tweets[2 + 6 * j]);
        printf("Comments:%s         Likes:%s\n", tweets[5 + 6 * j], tweets[4 + 6 * j]);
    }
}
void (*Like_Comment_page())(){
    printf("----------\n");
    printf("1.Like\n2.Comment\n3.See comments\n4.Back\n");
    char request[MAX_SIZE];
    scanf("%*c%[^\n]s",&request);
    switch (request[0]) {
        case '1':
            return Like_page;
            break;
        case '2':
            return Comment_page;
            break;
        case '3':
            return See_comments_page;
            break;
        case '4':
            return Menu_page;
            break;
        default:
            printf("Invalid request\nTry again\n");
            return Like_Comment_page;
            break;
    }
}
void (*Like_page())(){
    printf("----------\n");
    printf("Write tweet id:\n");
    char tweet_id[MAX_SIZE];
    scanf("%*c%[^\n]s",tweet_id);
    if(space_error(tweet_id)==true){
        printf("You can not use space in tweet id\n");
        return Like_Comment_page;
    }
    char message[MAX_SIZE]="like ";
    strcat(message, Token);
    strcat(message, ", ");
    strcat(message, tweet_id);
    strcat(message, "\n");
    char *feedback=send_data(message);
    Like_Comment_mes_analise(feedback);
    return Like_Comment_page;
}
void (*Comment_page())(){
    printf("----------\n");
    printf("Write tweet id:\n");
    char tweet_id[MAX_SIZE];
    scanf("%*c%[^\n]s",tweet_id);
    printf("Write your comment:\n");
    char comment[MAX_SIZE];
    scanf("%*c%[^\n]s",comment);
    char message[MAX_SIZE]="comment ";
    if(space_error(tweet_id)==true){
        printf("You can not use space in tweet id\n");
        return Like_Comment_page;
    }
    strcat(message, Token);
    strcat(message, ", ");
    strcat(message, tweet_id);
    strcat(message, ", ");
    strcat(message, comment);
    strcat(message, "\n");
    char *feedback=send_data(message);\
    Like_Comment_mes_analise(feedback);
    return Like_Comment_page;
}
void Like_Comment_mes_analise(char *input){
    char regex[]="\"type\":\"([^\n]+)\",\"message\":\"([^\n]+)\"";
    int num_caps=2;
    char caps[num_caps][100000];
    memset(caps, 0, num_caps*100000);
    slre_match(regex, input, 100000, caps, num_caps, 0);
    if(!strcmp(caps[0],"Error")){
        printf("%s\n",caps[1]);
        if(!strcmp(caps[1],"Bad request format.")){
            printf("You can only use numbers in tweet id.\n");
        }
    }
    else if(!strcmp(caps[0],"Successful")){
        printf("%s\n",caps[1]);
    }
}
void say_welcome_page(){
    printf("Welcome to Sweeter\n");
    /*srand( time( NULL ) );
    int index=rand()%7;
    char sentence[7][1000]={"Can I get a heal?"
                             ,"You must construct additional pylons.",
                             "We were expecting you :)",
                             "We hope you brought pizza.",
                             "Leave your weapons by the door.",
                             "Next time, bring a pepperoni pizza.",
                             "Too late I have been waiting a long time :("};
    printf("%s\n",sentence[index]);*/
}
void (*See_comments_page())(){
    printf("----------\n");
    printf("Write tweet id:\n");
    char tweet_id[MAX_SIZE];
    scanf("%*c%[^\n]s",tweet_id);
    printf("----------\nComments:\n");
    int i=0;
    while(i!=-1 && strcmp(tweets[i],tweet_id)){
        i=i+6;
        if(i>MAX_SIZE2){
            i=-1;
        }
    }
    if(i==-1){
        printf("Id not found\n");
        return Like_Comment_page;
    }
    else{
        char *tokenPtr1;
        char *tokenPtr2;
        char *tokenPtrfinal;
        char comments[MAX_SIZE2][MAX_SIZE2];
        memset(comments, 0, MAX_SIZE2*MAX_SIZE2);
        int size_tokenptr2=3;
        tokenPtr1=strstr(tweets[i+3],"\":\"");
        tokenPtrfinal=strstr(tweets[i+3],"}");
        int j=0;
        if(tokenPtr1!=NULL){
            tokenPtr2=strstr(tweets[i+3],"\"");
            tokenPtr2+=1;
        }
        while( tokenPtr1!=NULL) {
            if (tokenPtr1 < tokenPtrfinal) {
                strncat(comments[0 + 2 * j], tokenPtr2, tokenPtr1 - tokenPtr2);
                tokenPtr2 = strstr(tokenPtr1 + 3, "\",\"");
                if (tokenPtr2 == NULL /*|| tokenPtr2 > tokenPtrfinal*/) {
                    tokenPtr2 = tokenPtrfinal;
                    size_tokenptr2 = 1;
                }
                else{
                    tokenPtr2+=3;
                }
                strncat(comments[1 + 2 * j], tokenPtr1 + 3, tokenPtr2 - tokenPtr1 - 3 - size_tokenptr2);
                tokenPtr1 = strstr(tokenPtr2, "\":\"");
                j++;
            }
            else{
                break;
            }
        }
            if (j == 0) {
                printf("No comments\n");
            } else {
                for (int k = 0; k < j; k++) {
                    printf("%s: %s\n", comments[0 + 2 * k], comments[1 + 2 * k]);
                }
            }
    }
    return Like_Comment_page;
}
bool space_error(char * input){
    int i=0;
    while (input[i]!='\0'){
        if(input[i]==' '){
            return true;
        }
        i++;
    }
    return false;
}