#include <stdio.h>
#include <winsock2.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "slre.h"
#include <time.h>
#include <ctype.h>

#define MAX_SIZE 100000

int initialize (int port);
void start(int server_socket);
int check_username(char* username);
char* signup(char buffer[]);
void make_new_member(char* username , char* password);
void read_Admin_files();
int find_online_member(char* username);
int add_online_member(char* username);
bool check_password(int user_id,char* password);
char* login(char buffer[]);
char* logout(char buffer[]);
int member_id_with_token(char* token);
char* set_bio(char buffer[]);
char* change_password(char buffer[]);
void make_new_tweet(char* body,unsigned int owner_id);
char* send_tweet(char buffer[]);
char* make_comments_string(unsigned int number_of_comments,unsigned int id_of_comments[]);
char* member_username_with_id(unsigned int member_id);
char* make_one_tweet_string(unsigned int tweet_id);
char* make_tweets_string_for_profile(unsigned int number_of_tweets,unsigned int id_of_tweets[]);
char* profile(char buffer[]);
char* search(char buffer[]);
char* follow(char buffer[]);
char* unfollow(char buffer[]);
bool is_in(unsigned int value, unsigned int array[],unsigned int array_size);
char* refresh(char buffer[]);
char* like(char buffer[]);
void make_new_comment(char* comment_string , unsigned int owner);
char* comment(char buffer[]);
bool check_token(char* token);

unsigned int number_of_all_members;
unsigned int number_of_all_tweets;
unsigned int number_of_all_comments;
char online_members[1000][2][1000];

typedef struct {
    unsigned int id;
    char username[1000];
    char password[1000];
    char bio[1000];
    bool have_set_Bio;
    unsigned int number_of_followers;
    unsigned int id_of_followers[10000];
    unsigned int number_of_followings;
    unsigned int id_of_followings[10000];
    unsigned int number_of_tweets;
    unsigned int id_of_tweets[10000];
    unsigned int number_of_tweets_seen;
    unsigned int id_of_tweets_seen[10000];
}Member;

typedef struct {
    unsigned int id;
    char body[1000];
    unsigned int owner_id;
    unsigned int number_of_likes;
    unsigned int id_of_members_have_liked[10000];
    unsigned int number_of_comments;
    unsigned int id_of_comments[10000];
}Tweet;

typedef struct {
    unsigned int owner_id;
    unsigned int comment_id;
    char comment[1000];
}Comment;

bool startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
            lenstr = strlen(str);
    return lenstr < lenpre ? false : memcmp(pre, str, lenpre) == 0;
}

int main() {
    memset(online_members,0,sizeof (online_members));
    printf("Connecting...\n");
    read_Admin_files();
    int server_socket = initialize(12345);
    if(server_socket != INVALID_SOCKET){
        printf("Server is ready\n");
        start(server_socket);
    }
    else{
        printf("Error: INVALID SOCKET\n");
    }
    printf("End\n");
    return 0;
}
int initialize (int port){
    WSADATA wsadata;
    int wsa_res= WSAStartup(MAKEWORD(2,2),&wsadata);
    if(wsa_res != 0){
        printf("Failed to run wsadata\n");
        return INVALID_SOCKET;
    }
    int server_socket= socket(AF_INET,SOCK_STREAM,0);
    if(server_socket == INVALID_SOCKET){
        printf("Could not create socket\n");
        return INVALID_SOCKET;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family= AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int bind_res = bind(server_socket,(struct sockaddr*)&server_addr, sizeof(server_addr));
    if(bind_res == SOCKET_ERROR){
        printf("Bind failed\n");
        return INVALID_SOCKET;
    }

    listen(server_socket,SOMAXCONN);

    return server_socket;
}
void start(int server_socket){
    int len=sizeof(struct sockaddr_in);

    int client_socket;
    struct sockaddr_in client_addr;

    while((client_socket= accept(server_socket, (struct sockaddr*)& client_addr,&len)) != INVALID_SOCKET){
        printf("Client connected\n");
        char *buffer=malloc(MAX_SIZE);
        char* response;
        memset(buffer,0,MAX_SIZE);
        recv(client_socket, buffer, MAX_SIZE, 0);
        printf("Client request: %s",buffer);
        if(startsWith("signup ", buffer)){
            response=signup(buffer);
        }
        else if(startsWith("login ", buffer)){
            response=login(buffer);
        }
        else if(startsWith("logout ", buffer)){
            response=logout(buffer);
        }
        else if(startsWith("set bio ", buffer)){
            response=set_bio(buffer);
        }
        else if(startsWith("change password ", buffer)){
            response=change_password(buffer);
        }
        else if(startsWith("send tweet ", buffer)){
            response=send_tweet(buffer);
        }
        else if(startsWith("profile ", buffer)){
            response=profile(buffer);
        }
        else if(startsWith("search ", buffer)){
            response=search(buffer);
        }
        else if(startsWith("follow ", buffer)){
            response=follow(buffer);
        }
        else if(startsWith("unfollow ", buffer)){
            response=unfollow(buffer);
        }
        else if(startsWith("refresh ", buffer)){
            response=refresh(buffer);
        }
        else if(startsWith("like ", buffer)){
            response=like(buffer);
        }
        else if(startsWith("comment ", buffer)){
            response=comment(buffer);
        }
        else{
            response=(char*)malloc(100000);
            strcpy(response,"{\"type\":\"Error\",\"message\":\"Bad request format.\"}\n");
        }
        printf("Server response: %s",response);
        send(client_socket, response, strlen(response),0);
        closesocket(client_socket);
        free(response);
    }
}
char* signup(char buffer[]){
    char username[1000];
    char password[1000];
    char* response;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%s%s",username,password);
    username[strlen(username)-1]='\0';
    //printf("username: %s, password: %s\n",username,password);
    int check_answer=check_username(username);
    if(check_answer==-1){
        if(strlen(password)>16){
            strcpy(response,"{\"type\":\"Error\",\"message\":\"Your password can be up to 16 characters long.\"}\n");
        }
        else {
            strcpy(response, "{\"type\":\"Successful\",\"message\":\"You have signed in.\"}\n");
            make_new_member(username, password);
        }
    }
    else{
        strcpy(response,"{\"type\":\"Error\",\"message\":\"This username is already taken.\"}\n");
    }
    return response;
}
int check_username(char* username){
    int check=-1;
    int counter=1;
    char buffer[100000];
    char c;
    int i=0;
    FILE *handler = fopen(".\\Admin\\usernames.txt", "r");
    while((c=fgetc(handler))!=EOF){
        buffer[i]=c;
        i++;
    }
    buffer[i]='\0';
    fclose(handler);
    char* tokenptr;
    tokenptr=strtok(buffer,"\n");
    while (tokenptr != NULL)
    {
        if(strcmp(username,tokenptr)==0){
            check=counter;
            break;
        }
        tokenptr=strtok(NULL,"\n");
        counter++;
    }
    return check;
}
void make_new_member(char* username , char* password){
    number_of_all_members++;

    Member member;
    strcpy(member.username,username);
    strcpy(member.password,password);
    memset(member.bio,0,sizeof (member.bio));
    member.id=number_of_all_members;
    member.have_set_Bio=false;
    member.number_of_followers=0;
    memset(member.id_of_followers,0,sizeof (member.id_of_followers));
    member.number_of_followings=0;
    memset(member.id_of_followings,0,sizeof (member.id_of_followings));
    member.number_of_tweets=0;
    memset(member.id_of_tweets,0,sizeof (member.id_of_tweets));
    member.number_of_tweets_seen=0;
    memset(member.id_of_tweets_seen,0,sizeof (member.id_of_tweets_seen));

    char namefile[1000];
    sprintf(namefile,".\\Members\\%u.txt",number_of_all_members);
    FILE *outfile;
    outfile = fopen (namefile, "w");
    fwrite (&member, sizeof(Member), 1, outfile);
    fclose (outfile);

    FILE *outfile2;
    outfile2 = fopen (".\\Admin\\usernames.txt", "a");
    fprintf(outfile2,"%s\n",username);
    fclose (outfile2);

    FILE *f1 = fopen(".\\Admin\\members_number.txt", "w");
    fprintf(f1,"%u",number_of_all_members);
    fclose(f1);

}
void read_Admin_files(){
    FILE *f1 = fopen(".\\Admin\\members_number.txt", "r");
    fscanf(f1,"%u",&number_of_all_members);
    fclose(f1);
    FILE *f2 = fopen(".\\Admin\\tweets_number.txt", "r");
    fscanf(f2,"%u",&number_of_all_tweets);
    fclose(f2);
    FILE *f3 = fopen(".\\Admin\\comments_number.txt", "r");
    fscanf(f3,"%u",&number_of_all_comments);
    fclose(f3);
}
char* login(char buffer[]){
    char username[1000];
    char password[1000];
    char* token;
    char* response;
    int index;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%s%s",username,password);
    username[strlen(username)-1]='\0';
    //printf("username: %s, password: %s\n",username,password);
    int check_answer=check_username(username);
    if(check_answer==-1){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid username.\"}\n");
    }
    else{
        int check_online_answer=find_online_member(username);
        if(check_online_answer!=-1){
            strcpy(response,"{\"type\":\"Error\",\"message\":\"User in already login.\"}\n");
        }
        else{
            bool check_pass_answer=check_password(check_answer,password);
            if(check_pass_answer==false){
                strcpy(response,"{\"type\":\"Error\",\"message\":\"Incorrect password.\"}\n");
            }
            else{
                index=add_online_member(username);
                sprintf(response,"{\"type\":\"Token\",\"message\":\"%s\"}\n",online_members[index][1]);
            }
        }
    }
    return response;
}
int find_online_member(char* username){
    int check=-1;
    int i,index;
    for(i=0;i<1000;i++){
        if(strcmp(online_members[i][0],username)==0){
            return i;
        }
    }
    return check;
}
int add_online_member(char* username){
    int i,index;
    int flag=0;
    int ascii;
    for(i=0;i<1000;i++){
        if(online_members[i][0][0] == NULL){
            index=i;
            break;
        }
    }
    strcpy(online_members[index][0],username);
    char token[33];
    char randomchar[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h'
                     ,'i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                     'A','B','C','D','E','F','G','H','I','J','K','L','M',
                     'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    srand( time( NULL ) );
    while(flag==0){
        for(i=0;i<32;i++){
            ascii=rand()%62;
            token[i]=randomchar[ascii];
        }
        token[i]='\0';
        flag=1;
        for(i=0;i<1000;i++){
            if(strcmp(online_members[i][1],token)==0){
                flag=0;
            }
        }
    }
    strcpy(online_members[index][1],token);
    return index;
}
bool check_password(int user_id,char* password){
    char namefile[1000];
    sprintf(namefile,".\\Members\\%d.txt",user_id);
    FILE *infile;
    Member member;
    infile = fopen (namefile, "r");
    fread(&member, sizeof(Member), 1, infile);
    fclose (infile);
    //printf("mempas:%s\n",member.password);
    if(strcmp(member.password,password)==0){
        return true;
    }
    else{
        return false;
    }
}
char* logout(char buffer[]){
    char token[1000];
    char* response;
    int index,i;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%s",token);
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s\n",token);
    for(i=0;i<1000;i++){
        if(strcmp(token,online_members[i][1])==0){
            index=i;
            break;
        }
    }
    memset(online_members[index],0,sizeof(online_members[index]));
    strcpy(response,"{\"type\":\"Successful\",\"message\":\"You have loged out.\"}\n");
    return response;
}
char* set_bio(char buffer[]){
    char token[1000];
    char bio[1000];
    char* response;
    int index,i;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%*s%s%*c%[^\n]s",token,bio);
    token[32]='\0';
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s  Bio: %s\n",token,bio);
    int member_id=member_id_with_token(token);
    char namefile[1000];
    sprintf(namefile,".\\Members\\%d.txt",member_id);
    FILE *infile;
    Member member;
    infile = fopen (namefile, "r");
    fread(&member, sizeof(Member), 1, infile);
    fclose (infile);
    if(member.have_set_Bio==false){
        member.have_set_Bio=true;
        strcpy(member.bio,bio);
        FILE *outfile;
        outfile = fopen (namefile, "w");
        fwrite (&member, sizeof(Member), 1, outfile);
        fclose (outfile);
        strcpy(response,"{\"type\":\"Successful\",\"message\":\"Bio is updated successfully.\"}\n");
    }
    else{
        strcpy(response,"{\"type\":\"Error\",\"message\":\"You can only set your bio once.\"}\n");
    }
    return response;
}
int member_id_with_token(char* token){
    int i,index;
    for(i=0;i<1000;i++){
        if(strcmp(token,online_members[i][1])==0){
            index=i;
            break;
        }
    }
    int check_username_answer=check_username(online_members[index][0]);
    return check_username_answer;
}
char* change_password(char buffer[]){
    char token[1000];
    char currect_pass[1000];
    char new_pass[1000];
    char* response;
    int index,i;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%*s%s%*c%s%*c%s",token,currect_pass,new_pass);
    token[32]='\0';
    currect_pass[strlen(currect_pass)-1]='\0';
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s  currect: %s  new: %s\n",token,currect_pass,new_pass);
    int member_id=member_id_with_token(token);
    char namefile[1000];
    sprintf(namefile,".\\Members\\%d.txt",member_id);
    FILE *infile;
    Member member;
    infile = fopen (namefile, "r");
    fread(&member, sizeof(Member), 1, infile);
    fclose (infile);
    if(strcmp(member.password,currect_pass)!=0){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Entered current password is wrong.\"}\n");
    }
    else if(strcmp(new_pass,currect_pass)==0){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"New password is equal to current password.\"}\n");
    }
    else if(strlen(new_pass)>16){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Your password can be up to 16 characters long.\"}\n");
    }
    else{
        strcpy(member.password,new_pass);
        FILE *outfile;
        outfile = fopen (namefile, "w");
        fwrite (&member, sizeof(Member), 1, outfile);
        fclose (outfile);
        strcpy(response,"{\"type\":\"Successful\",\"message\":\"Password is changed successfully.\"}\n");
    }
    return response;
}
char* send_tweet(char buffer[]){
    char token[1000];
    char tweet[1000];
    char* response;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%*s%s%*c%[^\n]s",token,tweet);
    token[32]='\0';
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s  tweet: %s\n",token,tweet);
    unsigned int owner_id=member_id_with_token(token);
    make_new_tweet(tweet,owner_id);
    strcpy(response,"{\"type\":\"Successful\",\"message\":\"Tweet is sent successfully.\"}\n");
    return response;
}
void make_new_tweet(char* body,unsigned int owner_id){
    number_of_all_tweets++;

    Tweet tweet;
    strcpy(tweet.body,body);
    tweet.owner_id=owner_id;
    tweet.id=number_of_all_tweets;
    tweet.number_of_likes=0;
    memset(tweet.id_of_members_have_liked,0,sizeof (tweet.id_of_members_have_liked));
    tweet.number_of_comments=0;
    memset(tweet.id_of_comments,0,sizeof (tweet.id_of_comments));

    char namefile[1000];
    sprintf(namefile,".\\Tweets\\%u.txt",number_of_all_tweets);
    FILE *outfile;
    outfile = fopen (namefile, "w");
    fwrite (&tweet, sizeof(Tweet), 1, outfile);
    fclose (outfile);

    sprintf(namefile,".\\Members\\%u.txt",owner_id);
    Member member;
    FILE *infile;
    infile = fopen (namefile, "r");
    fread(&member, sizeof(Member), 1, infile);
    fclose (infile);
    member.id_of_tweets[member.number_of_tweets]=number_of_all_tweets;
    member.number_of_tweets++;

    FILE *outfile2;
    outfile2 = fopen (namefile, "w");
    fwrite (&member, sizeof(Member), 1, outfile2);
    fclose (outfile2);

    FILE *f1 = fopen(".\\Admin\\tweets_number.txt", "w");
    fprintf(f1,"%u",number_of_all_tweets);
    fclose(f1);

}
char* profile(char buffer[]){
    char token[1000];
    char * response;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%s",token);
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s\n",token);
    unsigned int member_id=member_id_with_token(token);

    Member member;
    char namefile[1000];
    sprintf(namefile,".\\Members\\%u.txt",member_id);
    FILE *infile;
    infile = fopen (namefile, "r");
    fread(&member, sizeof(Member), 1, infile);
    fclose (infile);

    char *tweets_string=make_tweets_string_for_profile(member.number_of_tweets,member.id_of_tweets);
    sprintf(response,"{\"type\":\"Profile\",\"message\":\"{\"username\":\"%s\",\"bio\":\"%s\",\"numberOfFollowers\""
                     ":%u,\"numberOfFollowings\":%u,\"followStatus\":\"Yourself\",\"allTweets\":[%s]}\"}\n",
                     member.username,member.bio,member.number_of_followers,member.number_of_followings,tweets_string);

    free(tweets_string);
    return response;
}
char* make_tweets_string_for_profile(unsigned int number_of_tweets,unsigned int id_of_tweets[]){
    char* tweets_string=(char*)calloc(100000,1);
    int i;
    for(i=0; i<number_of_tweets;i++){
        char* tweet=make_one_tweet_string(id_of_tweets[i]);
        strcat(tweets_string,tweet);
        free(tweet);
        if(i!=number_of_tweets-1){
            strcat(tweets_string,",");
        }
    }
    return tweets_string;
}
char* make_one_tweet_string(unsigned int tweet_id){
    char* tweet_string=(char*)calloc(100000,1);

    Tweet tweet;
    char namefile[1000];
    sprintf(namefile,".\\Tweets\\%u.txt",tweet_id);
    FILE *infile;
    infile = fopen (namefile, "r");
    fread(&tweet, sizeof(Tweet), 1, infile);
    fclose (infile);

    char* comments_string=make_comments_string(tweet.number_of_comments,tweet.id_of_comments);
    char*username=member_username_with_id(tweet.owner_id);

    sprintf(tweet_string,"{\"id\":%u,\"author\":\"%s\",\"content\":\"%s\",\"comments\":{%s},\"likes\":%u}\n",
            tweet.id,username,tweet.body,comments_string,tweet.number_of_likes);
    free(username);
    free(comments_string);
    return tweet_string;
}
char* member_username_with_id(unsigned int member_id){
    Member member;
    char namefile[1000];
    sprintf(namefile,".\\Members\\%u.txt",member_id);
    FILE *infile;
    infile = fopen (namefile, "r");
    fread(&member, sizeof(Member), 1, infile);
    fclose (infile);
    char * username=(char*)calloc(1000,1);
    strcpy(username,member.username);
    return username;
}
char* make_comments_string(unsigned int number_of_comments,unsigned int id_of_comments[]){
    char *comments_string=(char*)calloc(100000,1);
    char username[1000];
    char one_comment[100000];
    for(int i=0;i<number_of_comments;i++){
        memset(one_comment,0,sizeof(one_comment));
        unsigned int comment_id=id_of_comments[i];
        Comment comment;
        char namefile[1000];
        sprintf(namefile,".\\Comments\\%u.txt",comment_id);
        FILE *infile;
        infile = fopen (namefile, "r");
        fread(&comment, sizeof(Comment), 1, infile);
        fclose (infile);

        strcpy(username,member_username_with_id(comment.owner_id));
        sprintf(one_comment,"\"%s\":\"%s\"",username,comment.comment);
        strcat(comments_string,one_comment);
        if(i!=number_of_comments-1){
            strcat(comments_string,",");
        }
    }
    return comments_string;
}
char* search(char buffer[]){
    char token[1000];
    char username[1000];
    char * response;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%s%s",token,username);
    token[32]='\0';
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s  username for search: %s\n",token,username);

    int check_answer=check_username(username);
    if(check_answer==-1){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"This username is not valid.\"}\n");
    }
    else{
        unsigned int member_id=check_answer;
        unsigned int searching_member=member_id_with_token(token);

        Member member;
        char namefile[1000];
        sprintf(namefile,".\\Members\\%u.txt",member_id);
        FILE *infile;
        infile = fopen (namefile, "r");
        fread(&member, sizeof(Member), 1, infile);
        fclose (infile);

        bool rule=false;
        for(int i=0;i<member.number_of_followers;i++){
            if(member.id_of_followers[i]==searching_member){
                rule=true;
                break;
            }
        }
        char followstatus[]="NotFollowed";
        if(rule==true){
            strcpy(followstatus,"Followed");
        }

        char *tweets_string=make_tweets_string_for_profile(member.number_of_tweets,member.id_of_tweets);
        sprintf(response,"{\"type\":\"Profile\",\"message\":\"{\"username\":\"%s\",\"bio\":\"%s\",\"numberOfFollowers\""
                         ":%u,\"numberOfFollowings\":%u,\"followStatus\":\"%s\",\"allTweets\":[%s]}\"}\n",
                member.username,member.bio,member.number_of_followers,member.number_of_followings,followstatus,tweets_string);

        free(tweets_string);
    }
    return response;
}
char* follow(char buffer[]){
    char token[1000];
    char username[1000];
    char * response;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%s%s",token,username);
    token[32]='\0';
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s  username for follow: %s\n",token,username);
    unsigned int request_member_id=member_id_with_token(token);
    unsigned int member_to_follow_id=check_username(username);

    Member member;
    char namefile[1000];
    sprintf(namefile,".\\Members\\%u.txt",request_member_id);
    FILE *infile;
    infile = fopen (namefile, "r");
    fread(&member, sizeof(Member), 1, infile);
    fclose (infile);

    member.id_of_followings[member.number_of_followings]=member_to_follow_id;
    member.number_of_followings++;

    sprintf(namefile,".\\Members\\%u.txt",request_member_id);
    FILE *outfile;
    outfile = fopen (namefile, "w");
    fwrite (&member, sizeof(Member), 1, outfile);
    fclose (outfile);

    Member member2;
    sprintf(namefile,".\\Members\\%u.txt",member_to_follow_id);
    FILE *infile2;
    infile2 = fopen (namefile, "r");
    fread(&member2, sizeof(Member), 1, infile2);
    fclose (infile2);

    member2.id_of_followers[member2.number_of_followers]=request_member_id;
    member2.number_of_followers++;

    sprintf(namefile,".\\Members\\%u.txt",member_to_follow_id);
    FILE *outfile2;
    outfile2 = fopen (namefile, "w");
    fwrite (&member2, sizeof(Member), 1, outfile2);
    fclose (outfile2);

    char* request_username=member_username_with_id(member_to_follow_id);
    sprintf(response,"{\"type\":\"Successful\",\"message\":\"User %s is followed successfully.\"}\n",request_username);
    free(request_username);
    return response;
}
char* unfollow(char buffer[]){
    char token[1000];
    char username[1000];
    char * response;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%s%s",token,username);
    token[32]='\0';
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s  username for unfollow: %s\n",token,username);
    unsigned int request_member_id=member_id_with_token(token);
    unsigned int member_to_unfollow_id=check_username(username);

    Member member;
    char namefile[1000];
    sprintf(namefile,".\\Members\\%u.txt",request_member_id);
    FILE *infile;
    infile = fopen (namefile, "r");
    fread(&member, sizeof(Member), 1, infile);
    fclose (infile);

    int i;
    for(i=0;i<member.number_of_followings;i++){
        if(member.id_of_followings[i]==member_to_unfollow_id){
            break;
        }
    }
    for(int j=i;j<member.number_of_followings-1;j++){
        member.id_of_followings[j]=member.id_of_followings[j+1];
    }
    member.number_of_followings--;

    Member member2;
    sprintf(namefile,".\\Members\\%u.txt",member_to_unfollow_id);
    FILE *infile2;
    infile2 = fopen (namefile, "r");
    fread(&member2, sizeof(Member), 1, infile2);
    fclose (infile2);

    for(i=0;i<member2.number_of_followers;i++){
        if(member2.id_of_followers[i]==request_member_id){
            break;
        }
    }
    for(int j=i;j<member2.number_of_followers-1;j++){
        member2.id_of_followers[j]=member2.id_of_followers[j+1];
    }
    member2.number_of_followers--;

    sprintf(namefile,".\\Members\\%u.txt",member_to_unfollow_id);
    FILE *outfile2;
    outfile2 = fopen (namefile, "w");
    fwrite (&member2, sizeof(Member), 1, outfile2);
    fclose (outfile2);

    for(int i=0;i<member.number_of_tweets_seen;i++){
        if(is_in(member.id_of_tweets_seen[i],member2.id_of_tweets,member2.number_of_tweets)==true){
            for(int j=i;j<member.number_of_tweets_seen-1;j++){
                member.id_of_tweets_seen[j]= member.id_of_tweets_seen[j+1];
            }
            member.number_of_tweets_seen--;
            i--;
        }
    }

    sprintf(namefile,".\\Members\\%u.txt",request_member_id);
    FILE *outfile;
    outfile = fopen (namefile, "w");
    fwrite (&member, sizeof(Member), 1, outfile);
    fclose (outfile);

    char* request_username=member_username_with_id(member_to_unfollow_id);
    sprintf(response,"{\"type\":\"Successful\",\"message\":\"User %s is unfollowed successfully.\"}\n",request_username);
    free(request_username);
    return response;
}
char* refresh(char buffer[]){
    char token[1000];
    unsigned int array_of_id_to_show[10000];
    char * response;
    char * alltweets;
    response=(char*)malloc(1000);
    alltweets=(char*)calloc(1000,1);
    sscanf(buffer,"%*s%s",token);
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s\n",token);
    unsigned int member_id=member_id_with_token(token);

    Member member;
    char namefile[1000];
    sprintf(namefile,".\\Members\\%u.txt",member_id);
    FILE *infile;
    infile = fopen (namefile, "r");
    fread(&member, sizeof(Member), 1, infile);
    fclose (infile);

    int k=0;
    for(int i=0;i< member.number_of_followings;i++) {
        Member working_member;
        sprintf(namefile,".\\Members\\%u.txt",member.id_of_followings[i]);
        FILE *infile2;
        infile2 = fopen (namefile, "r");
        fread(&working_member, sizeof(Member), 1, infile2);
        fclose (infile2);
        for(int j=0;j<working_member.number_of_tweets;j++){
            if(is_in(working_member.id_of_tweets[j],member.id_of_tweets_seen,member.number_of_tweets_seen)==false){
                member.id_of_tweets_seen[member.number_of_tweets_seen]=working_member.id_of_tweets[j];
                member.number_of_tweets_seen++;
                array_of_id_to_show[k]=working_member.id_of_tweets[j];
                k++;
            }
        }
    }

    unsigned int hold;
    for(int i=0;i<k;i++){
        for(int j=0; j<k-1; j++){
            if(array_of_id_to_show[j]>array_of_id_to_show[j+1]){
                hold=array_of_id_to_show[j];
                array_of_id_to_show[j]=array_of_id_to_show[j+1];
                array_of_id_to_show[j+1]=hold;
            }
        }
    }

    for(int i=0;i<k;i++){
        char* tweet_string=make_one_tweet_string(array_of_id_to_show[i]);
        strcat(alltweets,tweet_string);
        strcat(alltweets,",");
        free(tweet_string);
    }

    if(strlen(alltweets)!=0){
        alltweets[strlen(alltweets)-1]='\0';
    }

    sprintf(namefile,".\\Members\\%u.txt",member_id);
    FILE *outfile;
    outfile = fopen (namefile, "w");
    fwrite (&member, sizeof(Member), 1, outfile);
    fclose (outfile);

    sprintf(response,"{\"type\":\"List\",\"message\":[%s]}\n",alltweets);
    return response;
}
bool is_in(unsigned int value, unsigned int array[],unsigned int array_size){
    for(int i=0;i< array_size; i++){
        if(array[i]==value){
            return true;
        }
    }
    return false;
}
char* like(char buffer[]){
    char token[1000];
    char string_tweet_id[100000];
    char * response;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%s%s",token,string_tweet_id);
    token[32]='\0';
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    //printf("token: %s  tweet id: %s\n",token,string_tweet_id);

    for(int i=0;i<strlen(string_tweet_id);i++){
        if(isdigit(string_tweet_id[i])==0){
            strcpy(response,"{\"type\":\"Error\",\"message\":\"Bad request format.\"}\n");
            return response;
        }
    }

    unsigned int tweet_id;
    sscanf(string_tweet_id,"%u",&tweet_id);

    if(tweet_id>number_of_all_tweets){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n");
        return response;
    }

    Tweet tweet;
    char namefile[1000];
    sprintf(namefile,".\\Tweets\\%u.txt",tweet_id);
    FILE *infile;
    infile = fopen (namefile, "r");
    fread(&tweet, sizeof(Tweet), 1, infile);
    fclose (infile);

    unsigned int member_id=member_id_with_token(token);

    if(is_in(member_id,tweet.id_of_members_have_liked,tweet.number_of_likes)==true){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"You can only like a tweet once.\"}\n");
        return response;
    }

    tweet.id_of_members_have_liked[tweet.number_of_likes]=member_id;
    tweet.number_of_likes++;

    FILE *outfile;
    outfile = fopen (namefile, "w");
    fwrite (&tweet, sizeof(Tweet), 1, outfile);
    fclose (outfile);

    sprintf(response,"{\"type\":\"Successful\",\"message\":\"Tweet %u successfully liked.\"}\n",tweet_id);
    return response;
}
char* comment(char buffer[]){
    char token[1000];
    char string_tweet_id[100000];
    char comment[1000];
    char * response;
    response=(char*)malloc(100000);
    sscanf(buffer,"%*s%s%s%*c%[^\n]s",token,string_tweet_id,comment);
    token[32]='\0';
    if(check_token(token)==false){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Invalid token.\"}\n");
        return response;
    }
    string_tweet_id[strlen(string_tweet_id)-1]='\0';
    //printf("token: %s  tweet id: %s  comment: %s\n",token,string_tweet_id,comment);

    for(int i=0;i<strlen(string_tweet_id);i++){
        if(isdigit(string_tweet_id[i])==0){
            strcpy(response,"{\"type\":\"Error\",\"message\":\"Bad request format.\"}\n");
            return response;
        }
    }

    unsigned int tweet_id;
    sscanf(string_tweet_id,"%u",&tweet_id);

    if(tweet_id>number_of_all_tweets){
        strcpy(response,"{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n");
        return response;
    }

    Tweet tweet;
    char namefile[1000];
    sprintf(namefile,".\\Tweets\\%u.txt",tweet_id);
    FILE *infile;
    infile = fopen (namefile, "r");
    fread(&tweet, sizeof(Tweet), 1, infile);
    fclose (infile);

    unsigned int member_id=member_id_with_token(token);
    make_new_comment(comment,member_id);
    tweet.id_of_comments[tweet.number_of_comments]=number_of_all_comments;
    tweet.number_of_comments++;

    FILE *outfile;
    outfile = fopen (namefile, "w");
    fwrite (&tweet, sizeof(Tweet), 1, outfile);
    fclose (outfile);

    sprintf(response,"{\"type\":\"Successful\",\"message\":\"Comment is sent successfully.\"}\n",tweet_id);
    return response;
}
void make_new_comment(char* comment_string , unsigned int owner){
    number_of_all_comments++;

    Comment comment;
    strcpy(comment.comment,comment_string);
    comment.comment_id=number_of_all_comments;
    comment.owner_id=owner;

    char namefile[1000];
    sprintf(namefile,".\\Comments\\%u.txt",number_of_all_comments);
    FILE *outfile;
    outfile = fopen (namefile, "w");
    fwrite (&comment, sizeof(Comment), 1, outfile);
    fclose (outfile);

    FILE *f1 = fopen(".\\Admin\\comments_number.txt", "w");
    fprintf(f1,"%u",number_of_all_comments);
    fclose(f1);

}
bool check_token(char* token){
    int i,index;
    for(i=0;i<1000;i++){
        if(strcmp(token,online_members[i][1])==0){
            return true;
            break;
        }
    }
    if(i==1000){
        return false;
    }
}