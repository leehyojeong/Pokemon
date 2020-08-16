#include <stdio.h>

#include <sys/types.h>

#include <sys/wait.h>

#include <unistd.h>

#include <string.h>

#include <errno.h>

#include <fcntl.h>

#include <sys/stat.h>

#include <string.h>

#include <stdlib.h>

#include <time.h>

#include <dirent.h>

 

 

//readline

int readline(int fd, char *buf, int nbytes) {

   int numread = 0;

   int returnval;

   while (numread < nbytes - 1) {

      returnval = read(fd, buf + numread, 1);

      if ((returnval == -1) && (errno == EINTR))

         continue;

      if ((returnval == 0) && (numread == 0))

         return 0;

      if (returnval == 0)

         break;

      if (returnval == -1)

         return -1;

      numread++;

      if (buf[numread - 1] == '\n') {

         buf[numread] = '\0';

         return numread;

      }

   }

   errno = EINVAL;

   return -1;

}

//gotoxy

void gotoxy(int x, int y) {

   printf("%c[%d;%df", 0x1B, y, x);

}

//rmdirs

int rmdirs(const char *path, int is_error_stop) {

   DIR* dir_ptr = NULL;

   struct dirent *file = NULL;

   struct stat buf;

   char filename[1024];

   if ((dir_ptr = opendir(path)) == NULL) {

      return unlink(path);

   }

   while ((file = readdir(dir_ptr)) != NULL) {

      if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {

         continue;

      }

      sprintf(filename, "%s/%s", path, file->d_name);

      if (lstat(filename, &buf) == -1) {

         continue;

      }

      if (S_ISDIR(buf.st_mode)) {

         if (rmdirs(filename, is_error_stop) == -1 && is_error_stop) {

            return -1;

         }

      }

      else if (S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)) {

         if (unlink(filename) == -1 && is_error_stop) {

            return 1;

         }

      }

   }

   closedir(dir_ptr);

   return rmdir(path);

}

// end of rmdirs

#define READ_FLAGS O_RDONLY

#define max_level 20

#define max_pp 15

#define map_size 10

#define max_poke 4

#define base_hp 20

#define base_attack 10

enum who {

   boss,

   npc1, npc2, npc3, npc4,

   wild1, wild2, wild3, wild4, wild5, wild6, wild7, wild8, wild9, wild10

};

enum item {

   medicine, m_ball, pp_max

};

const int lv_hp[20]

= { 20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,120 };

const int lv_attack[21]

= { 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30 };

struct SKILL {

   char* name;

   int power;

   int pp;

}typedef Skill;

struct POKEMON {

   char*name;

   int max_hp;

   int current_hp;

   double level;

   Skill skill[4];

   int attack;

}typedef Pokemon;

struct USER {

   Pokemon pokemon[4];

   int item[3];

   char* name;

   int x, y;

   int npc_fight[4];

   int map;

}typedef User;

int map1[map_size][map_size];

int map2[map_size][map_size];

User npc[4];

//main

User Lim;

Pokemon Poke_Dataset[10];

Skill Skill_Dataset[10];

void Game();

int Menu();

void Init(User* u);

void LoadData(User* u);

void SaveData(User* u);

void LoadMap(int map[10][10], const char* Path);

int move(int x, int y, int map[map_size][map_size]);

int fight(User* u, User* npc, int who);

void show_pstate(Pokemon* my_p, Pokemon* fight_p);

void Center(User* u);

int p_abandon(User* u);

int p_catch(User* u, Pokemon* p);

void show_inven(User* u);

void show_plist(User* u);

void change_pokemon();

void show_ending();

void move_dir();

void copy_data(Pokemon* p1, Pokemon* p2);

void Poke_Set();

void show_map(User* u);

void LevelUp(Pokemon* p);

void start(User *u);

void copy_skilldata(Skill* skill1, Skill* skill2);

void Poke_Set() {

   Skill_Dataset[0].name = "Splash";

   Skill_Dataset[1].name = "Pound";

   Skill_Dataset[2].name = "QuickAttack";

   Skill_Dataset[3].name = "Blaze";

   Skill_Dataset[4].name = "WaterGun";

   Skill_Dataset[5].name = "VineWhip";

   Skill_Dataset[6].name = "ThunderBolt";

   Skill_Dataset[7].name = "AncientPower";

   Skill_Dataset[8].name = "SolarBeam";

   Skill_Dataset[9].name = "HyperBeam";

   for (int i = 0; i < 10; i++) {

      if (i > 0) {

         Skill_Dataset[i].power = (int)(2 + i * 0.4);

      }

      else {

         Skill_Dataset[0].power = 0;

      }

      Skill_Dataset[i].pp = max_pp;

   }

   for (int i = 0; i < 10; i++) {

      Poke_Dataset[i].attack = base_attack;

      Poke_Dataset[i].current_hp = base_hp;

      Poke_Dataset[i].level = 1;

      Poke_Dataset[i].max_hp = base_hp;

      for (int j = 0; j < 4; j++) {

         Poke_Dataset[i].skill[j] = Skill_Dataset[rand() % 4];

         for (int k = 0; k < j; k++) {

            if (Poke_Dataset[i].skill[j].name == Poke_Dataset[i].skill[k].name)

               j--;

         }

      }

   }

   Poke_Dataset[0].name = "Pikachu";

   Poke_Dataset[1].name = "Caterpie";

   Poke_Dataset[2].name = "Zubat";

   Poke_Dataset[3].name = "Paras";

   Poke_Dataset[4].name = "Ponyta";

   Poke_Dataset[5].name = "Chikorita";

   Poke_Dataset[6].name = "Togepi";

   Poke_Dataset[7].name = "Aipom";

   Poke_Dataset[8].name = "Wooper";

   Poke_Dataset[9].name = "Mew";

}

void npc_set() {

   srand((unsigned)time(NULL));

   npc[0].name = "ChanGu";

   npc[1].name = "JaeMin";

   npc[2].name = "SeJeong";

   npc[3].name = "HyoJeong";

   npc[0].pokemon[0].name = "MetaMone";

   npc[1].pokemon[0].name = "ESangHaeSee";

   npc[2].pokemon[0].name = "NaMolBaeMi";

   npc[3].pokemon[0].name = "Liako";

   for (int i = 0; i < 4; i++)

   {

      npc[i].pokemon[0].level = 5 + (i * 5);

      npc[i].pokemon[0].max_hp = lv_hp[4 + (i * 5)];

      npc[i].pokemon[0].current_hp = lv_hp[4 + (i * 5)];

      npc[i].pokemon[0].attack = lv_attack[4 + (i * 5)];

      for (int j = 0; j < 4; j++)

         npc[i].pokemon[0].skill[j] = Skill_Dataset[rand() % 10];

   }

   Lim.name = "Prof.Lim";

   Lim.pokemon[0].name = "";

   Lim.pokemon[0].level = 20;

   Lim.pokemon[0].max_hp = base_hp;

   Lim.pokemon[0].current_hp = base_hp;

   Lim.pokemon[0].attack = base_attack;

   for (int i = 0; i< 4; i++) {

      Lim.pokemon[0].skill[i] = Skill_Dataset[rand() % 10];

   }

}

void main() {

   pid_t id;

   id = fork();

   int stat_loc;

   if (id == -1) {

      perror("fail to fork\n");

      return;

   }

   else if (id == 0) {

      Game();

   }

   else {

      wait(&stat_loc);

      printf("\e[2J\e[H");

      exit(0);

   }

}

void Game() {

   printf("\e[2J\e[H");//clear

   srand(time(NULL));

   User* u = (User*)malloc(sizeof(User));

   pid_t id;

   id = fork();

   int bossId;

   int stat_loc;

   int menu;

   int x, y;

   int win = 0;

   char mycwd[5];

   char dirname[25];

   int npc_index = 0;

   //to win

   //

   Poke_Set();

   if (id == -1) {

      perror("fail to fork\n");

      return;

   }

   else if (id == 0) {

      menu = Menu();

   }

   else {

      wait(&stat_loc);

      exit(0);

   }

   npc_set();

   switch (menu) {

   case 1:

      Init(u);

      printf("\e[2J\e[H");

      start(u);

      break;

   case 2:

      LoadData(u);

      u->pokemon[0].level -= 1;      // from a

      LevelUp(&u->pokemon[0]);   // to a

      break;

   case 3:

      rmdirs("./fLim", 1);//delete directory

      exit(0);

      return;

      break;

   }

   while (1) {

      //delete

      for (int i = 0; i<4; i++) {

         printf("in while skill %s\n", u->pokemon[0].skill[i].name);

      }

      //SaveData(u);

      show_map(u);

      gotoxy(23, 19);

      printf("Select menu");

      gotoxy(23, 20);

      printf("1.move");

      gotoxy(23, 21);

      printf("2.show my Pokemon");

      gotoxy(23, 22);

      printf("▶  ");

      int sel;

      scanf("%d", &sel);

      switch (sel) {

      case 1:

         gotoxy(23, 24);

         printf("Please enter x, y to move:");

         scanf("%d %d", &y, &x);

         int moveR;

         if (u->map == 1)

            moveR = move(u->x + x, u->y + y, map1);

         else

            moveR = move(u->x + x, u->y + y, map2);

         if (moveR != -1)

         {

            u->x = u->x + x;

            u->y = u->y + y;

         }

         switch (moveR)

         {

         case -1:

            printf("The end of Map\n");

            break;

         case -2:

            u->map += 1;

            printf("Next Town!\n");

            sleep(3);

            break;

         case 0://normal

            break;

         case 1:

         case 2:

         case 3:

         case 4:

         case 6:

            if (moveR<6)

               npc_index = moveR - 1;

            else

               npc_index = 0;

            fight(u, &npc[npc_index], moveR);

            SaveData(u);

            break;

         case 7:

            printf("We're in the Pokemon Center!\n");

            Center(u);

         }

         break;

      case 2:

         show_plist(u);

         break;

      }

      for (win = 0; win < 4; win++) {      // from lim

         if (u->npc_fight[win] == 0) {

            win = 0;

            break;

         }

      }

      if (win != 0) {

         printf("\e[2J\e[H");

         printf("You Eliminate all NPCs.\n");

         printf("Find Boss.\n");

         printf("Boss is near you.\n");

         strcpy(mycwd, "fLim");

         getcwd(mycwd, 15);

         strcpy(dirname, mycwd);

         strcat(dirname, "5418Lee");

         mkdir(dirname, 0775);

         strcpy(dirname, mycwd);

         strcat(dirname, "9751Jeon");

         mkdir(dirname, 0775);

         strcpy(dirname, mycwd);

         strcat(dirname, "5377Lee");

         mkdir(dirname, 0775);

         strcpy(dirname, mycwd);

         strcat(dirname, "6120Baek");

         mkdir(dirname, 0775);

         strcpy(dirname, mycwd);

         strcat(dirname, "6270Lim");

         mkdir(dirname, 0775);

         while (1) {

            printf("Where is Boss?");

            scanf("%d", &bossId);

            if (bossId == 6270) {

               if (fight(u, &Lim, boss) == 1) {

                  printf("Found!!\n");

                  break;

               }

            }

         }

      }                           // to lim

   }

}

int Menu() {

   gotoxy(13, 6);

   printf("            ==************#+=            \n");

   gotoxy(13, 7);

   printf("         .=.    :*+++++++++**#+.         \n");

   gotoxy(13, 8);

   printf("        ++      =*+++++++++++++#+        \n");

   gotoxy(13, 9);

   printf("       **+.  .:**+++*#####*+++++**       \n");

   gotoxy(13, 10);

   printf("      =*++*********##+:..:*#*++++*=     1.new adventure\n");

   gotoxy(13, 11);

   printf("      =*++*********##+:..:*#*++++*=      \n");

   gotoxy(13, 12);

   printf("      #+****#########      ####**+=     2.continue\n");

   gotoxy(13, 13);

   printf("      ########**++=*#=.  .++######=      \n");

   gotoxy(13, 14);

   printf("      ##*++.        =*####*.  .:=+=     3.exit\n");

   gotoxy(13, 15);

   printf("      :+====.                     :      \n");

   gotoxy(13, 16);

   printf("       =+====:                  .+      please select menu: \n");

   gotoxy(13, 17);

   printf("        :*=====:              .+=        \n");

   gotoxy(13, 18);

   printf("          =*======::..   ..:=*+          \n");

   gotoxy(13, 19);

   printf("            :=+*+=======+*+=:            \n");

   gotoxy(13, 20);

   printf("                .:::::::.                \n");

   //gotoxy(20,22);

   //printf("please select menu : ");

   gotoxy(72, 16);

   int menu_num;

   scanf("%d", &menu_num);

   return menu_num;

}

void LoadData(User* u) {

   int fd = open("fileData.txt", O_RDWR);

   char str[1024];

   char* buf;

   char* delimeter = "/";

   if (fd == -1) { printf("LoadData error\n"); }

   fd = readline(fd, str, sizeof(str));

   // printf("%s", str);

   close(fd);

   LoadMap(map1, "map1.txt");

   LoadMap(map2, "map2.txt");

   //  printf("원본: %s\n", str);

   // printf("== 공백이나 콤마, 느낌표, 마침표를 기준으로 분할 ==\n");

   buf = strtok(str, delimeter);  //처음 호출 시에 대상 문자열 전달

   u->x = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->y = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->map = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->item[medicine] = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->item[m_ball] = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->item[pp_max] = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->npc_fight[0] = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->npc_fight[1] = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->npc_fight[2] = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->npc_fight[3] = atoi(buf);

   buf = strtok(NULL, delimeter);

   u->name = (char*)malloc(strlen(buf) * sizeof(char));

   strcpy(u->name, buf);

   for (int i = 0; i < 4; i++) {

      buf = strtok(NULL, delimeter);

      u->pokemon[i].attack = atoi(buf);

      buf = strtok(NULL, delimeter);

      u->pokemon[i].current_hp = atoi(buf);

      buf = strtok(NULL, delimeter);

      u->pokemon[i].level = atof(buf);

      buf = strtok(NULL, delimeter);

      u->pokemon[i].max_hp = atoi(buf);

      buf = strtok(NULL, delimeter);

      u->pokemon[i].name = (char*)malloc(strlen(buf) * sizeof(char));

      strcpy(u->pokemon[i].name, buf);

      for (int j = 0; j < 4; j++) {

         buf = strtok(NULL, delimeter);

         u->pokemon[i].skill[j].name = (char*)malloc(strlen(buf) * sizeof(char));

         strcpy(u->pokemon[i].skill[j].name, buf);

         buf = strtok(NULL, delimeter);

         u->pokemon[i].skill[j].power = atoi(buf);

         buf = strtok(NULL, delimeter);

         u->pokemon[i].skill[j].pp = atoi(buf);

      }

   }

   printf("Loading now...\n");

   u->pokemon[0].level--;

   LevelUp(&(u->pokemon[0]));

   //sleep(2); 

   // printf("%d\n", u->pokemon[0].max_hp);

   // printf("%d\n", u->pokemon[1].max_hp);

   //  printf("%d\n", u->pokemon[2].max_hp);

   // printf("%d\n", u->pokemon[3].max_hp);

   printf("Loading finish...\n");

   //sleep(2);

}

void SaveData(User* u) {

   printf("Saving Data....\n");

   int fd = open("fileData.txt", O_WRONLY);

   char buf[512] = "";

   if (fd == -1) { printf("SaveData error\n"); }

   sprintf(buf, "%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%s", u->x, u->y, u->map, u->item[medicine], u->item[m_ball], u->item[pp_max], u->npc_fight[0], u->npc_fight[1], u->npc_fight[2], u->npc_fight[3], u->name);

   printf("Wait Second...\n");

   write(fd, buf, strlen(buf));

   for (int i = 0; i < 4; i++)

   {

      sprintf(buf, "/%d/%d/%lf/%d/%s", u->pokemon[i].attack, u->pokemon[i].current_hp, u->pokemon[i].level, u->pokemon[i].max_hp, u->pokemon[i].name);

      write(fd, buf, strlen(buf));

      for (int j = 0; j < 4; j++)

      {

         sprintf(buf, "/%s/%d/%d", u->pokemon[i].skill[j].name, u->pokemon[i].skill[j].power, u->pokemon[i].skill[j].pp);

         write(fd, buf, strlen(buf));

      }

   }

   printf("Save all Data!\n");

   close(fd);

}

void start(User* u) {

   char name[15];

   printf("Mmm!!\n\n");

   printf("Welcome to the world of Pokemon!\n\n");

   sleep(1);

   printf("\e[2J\e[H");

   printf("My name is Prof.Lim!\n\n");

   printf("Everyone`s calling it Dr. Pokemon.\n\n");

   printf("You`re going to start with the world.\n\n");

   printf("in the lead\n\n");

   sleep(2);

   printf("\e[2J\e[H");

   printf("You`re going on an adventure.\n\n");

   printf("in the street, in the house, on the road, in the grass.\n\n");

   printf("And in caves, in forests...\n\n");

   printf("Sometimes people are not going to be able to do it.\n\n");

   printf("Someone will challenge you\n\n");

   printf("Pokemons in the wild\n\n");

   printf("There will also be roadblocks.\n\n");

   sleep(2);

   printf("\e[2J\e[H");

   printf("In the process of fighting them and winning them,\n\n");

   printf("You`ll get stronger and stronger.\n\n");

   printf("But being strong isn`t the only purpose of adventure.\n\n");

   sleep(2);

   printf("\e[2J\e[H");

   printf("We humans\n\n");

   printf("live in harmony with Pokemon.\n\n");

   printf("We play together.\n\n");

   printf("They work together.\n\n");

   printf("And let Pokemon fight\n\n");

   printf("He`s strengthening his bond...\n\n");

   printf("I want to know those Pokemon in detail.\n\n");

   printf("I`m working on it!\n\n");

   sleep(2);

   printf("\e[2J\e[H");

   printf("Anyway,\n\n");

   printf("let`s find out about you now!\n\n");

   printf("(Name is no longer than 15 characters and does not contain spaces.\n)");

   scanf("%s", name);

   strlen(name);

   u->name = (char*)malloc(strlen(name) * sizeof(char));

   strcpy(u->name, name);

   printf("\e[2J\e[H");

   printf("%s!!\n\n", u->name);

   printf("from now on\n\n");

   printf("The story begins!\n\n");

   printf("There, you will meet\n\n");

   printf("many Pokemon\n\n");

   printf("or many people to find something!\n\n");

   printf("Then in the world of Pokemon!\n\n");

   sleep(2);

   printf("\e[2J\e[H");

   return;

}

void Init(User* u) {

   extern int errno;

   //Poke_Set();

   u->name = "Unknown"; //Input 받아야 함

   u->pokemon[0].name = "Pikachu";

   u->pokemon[0].level = 1;

   u->pokemon[0].current_hp = base_hp;

   u->pokemon[0].max_hp = base_hp;

   u->pokemon[0].attack = base_attack;

   u->map = 1;//map1

   for (int i = 1; i < 4; i++) {

      u->pokemon[i].name = "nothing";

      u->pokemon[i].level = 1;

      u->pokemon[i].current_hp = 0;

      u->pokemon[i].attack = base_attack;

   }

   u->x = 5;      // 임의 지정

   u->y = 5;

   for (int i = 0; i < 4; i++) {

      u->npc_fight[i] = 0;

   }

   u->item[0] = 5;

   u->item[1] = 10;

   u->item[2] = 5;

   for (int i = 0; i < 4; i++) {

      for (int j = 0; j < 4; j++) {

         u->pokemon[i].skill[j] = Skill_Dataset[rand() % 10];

      }

   }

   LoadMap(map1, "map1.txt");

   LoadMap(map2, "map2.txt");

   int fd = open("fileData.txt", O_CREAT | O_EXCL, S_IRWXU | S_IRGRP | S_IROTH);

   if (fd == -1 || errno) printf("Exist File\n");

   close(fd);

}

void LoadMap(int map[10][10], const char* Path) {

   //  printf("Start LoadMap\n"); 

   int fd = open(Path, O_RDWR);

   if (fd == -1) { printf("LoadMap error\n"); }

   else {

      char buf[1024];

      read(fd, buf, 1024);

      char *ret_ptr;

      char *next_ptr;

      int i = 0;

      int j = 0;

      ret_ptr = strtok_r(buf, " \n", &next_ptr);

      while (ret_ptr) {

         map[i][j++] = atoi(ret_ptr);

         if (j == map_size) {

            i++;

            j = 0;

         }

         ret_ptr = strtok_r(NULL, " \n", &next_ptr);

      }

      close(fd);

   }

}

void copy_data(Pokemon* p1, Pokemon* p2) {

   p1->attack = p2->attack;

   p1->current_hp = p2->current_hp;

   p1->level = p2->level;

   p1->max_hp = p2->max_hp;

   p1->name = p2->name;

   for (int i = 0; i < 4; i++) {

      copy_skilldata(&(p1->skill[i]), &(p2->skill[i]));

   }

}

void copy_skilldata(Skill* skill1, Skill* skill2) {

   skill1->name = skill2->name;

   skill1->power = skill2->power;

   skill1->pp = skill2->pp;

}

int move(int x, int y, int map[map_size][map_size]) {

   srand(time(NULL));

   if (x >= 0 && x< map_size && y >= 0 && y< map_size)

   {

      if (map[x][y] == -1)

      {

         int random = rand() % 2;

         if (random == 0)

            return 0;

         else

            return 6;

      }

      return map[x][y];

   }

   else return -1;

}

 

void show_map(User* u)

{

   printf("\e[2J\e[H");//clear

   gotoxy(0, 8);

   for (int i = 0; i<map_size; i++)

   {

      gotoxy(25, 8 + i);

      for (int j = 0; j<map_size; j++)

      {

         if (u->x == i && u->y == j)

         {

            printf("■  ");//user

         }

         else {

            if (u->map == 1) {

               if ((map1[i][j] >= 1 && map1[i][j] <= 4))

                  printf("★  ");//npc

               else if (map1[i][j] == -2)

                  printf("▷  ");//next map

               else if (map1[i][j] == 7)

                  printf("▣  ");//center

               else

                  printf("□  ");//normal(wild)

            }

            else if (u->map == 2) {

               if (map2[i][j] >= 1 && map2[i][j] <= 4)

                  printf("★  ");//npc

               else if (map2[i][j] == -2)

                  printf("◁  ");//next map

               else

                  printf("□  ");//normal(wild)

            }

         }

      }

      printf("\n");

   }

}

void show_pstate(Pokemon* my_p, Pokemon* fight_p) {

   gotoxy(30, 3);

   printf("************************ ************************");

   gotoxy(30, 4);

   printf("|          YOU         | |        OPPONENT      |");

   gotoxy(30, 5);

   printf("|%10s HP %3d/%3d | |%10s HP %3d/%3d |"

      , my_p->name, my_p->current_hp, my_p->max_hp,

      fight_p->name, fight_p->current_hp, fight_p->max_hp);

   gotoxy(30, 6);

   printf("************************ ************************");

   printf("\n");

}

int p_abandon(User* u) {

   int input;

   char input2;

   Pokemon p;

   {

      p.attack = 0;

      p.current_hp = 0;

      p.level = 0;

      p.name = "nothing";

      p.skill->name = NULL;

      p.skill->power = 0;

      p.skill->pp = 0;

   }

   while (1) {

      printf("select pokemon to release(execpt 0) :");

      scanf("%d", &input);

      if (input < 4 && input > 0) {

         //copy_data(&u->pokemon[input], &p);

         printf("%s was released...:( \n", u->pokemon[input].name);

         printf("\e[2J\e[H");

         copy_data(&u->pokemon[input], &p);

         return 1;

      }

      else {

         printf("Invalid Value!\n");

         printf("Do you want to reenter?(y / ohter) :");

         getchar();

         scanf("%c", &input2);

         if (input2 != 'y')

            return 0;

      }

   }

}

int p_catch(User* u, Pokemon* p) {//success 1 fail 0

   int pro, cat = 0;

   char input;

   srand(time(NULL));

   for (int i = 0; i < max_poke; i++) {

      if (!strcmp(u->pokemon[i].name, "nothing") && u->item[m_ball]>0) {

         u->item[m_ball]--;

         pro = rand() % 101;

         cat = ((p->max_hp - p->current_hp) * 100) / p->max_hp;

         if (pro <= cat) {

            gotoxy(30, 10);

            printf("You catch %s!\n", p->name);

            sleep(2);

            copy_data(&u->pokemon[i], p);

            u->pokemon[i].name = p->name;

            u->pokemon[i].level = 1;

            u->pokemon[i].current_hp = base_hp;

            u->pokemon[i].attack = base_attack;

            return 1;

         }

         else {

            printf("Failed to capture Pokemon!! \n");

            return 0;

         }

      }

      else if (u->item[m_ball]>0 && strcmp(u->pokemon[3].name, "nothing")) {

         printf("I can't own Pokemon anymore.\n");

         printf("Would you like to throw away the Pokemon you have?(y / other)\n");

         getchar();

         scanf("%c", &input);

         if (input == 'y') {

            if (!p_abandon(u)) {

               //   printf("\e[2J\e[H");

               p_catch(u, p);

            }

         }

         else {

            return 0;

         }

      }

      else if (u->item[m_ball] < 1) {

         printf("You don't have enough monster ball.\n");

         return 0;

      }

   }

}

//void change_pokemon(User* u) {

//   int input;

//   char input2;

//   Pokemon p;

//   while (1) {

//      printf("select pokemon to change(except 0)  :");

//      scanf("%d", &input);

//      if (input < 4 && input > 0) {

//         copy_data(&p, &u->pokemon[input]);

//         copy_data(&u->pokemon[input], &u->pokemon[0]);

//         copy_data(&u->pokemon[0], &p);

//         return 1;

//      }

//      else {

//         printf("Invalid Value!\n");

//         printf("Do you want to reenter? (y / ohter) :");

//         scanf("%c", &input2);

//         if (input2 == 'y')

//            continue;

//         else

//            return 0;

//      }

//   }

//}

int fight(User* u, User* npc, int who) {//win = 1, lose = 0

   int select1 = 0;

   int select_skill = 0;

   int select_item = 0;

   int inum, snum, random_skill;

   int catch_rate;

   int wild_rand;

   Pokemon my_p;//My Pokemon

   Pokemon fight_p;//Fight Pokemon

   copy_data(&my_p, &(u->pokemon[0]));

   if (who == boss || u->npc_fight[who - 1] == 0)

   {

      if (who == boss) {//boss

         printf("\e[2J\e[H");//clear map

         copy_data(&fight_p, &(npc->pokemon[0]));

         printf("%s would like to battle!\n", npc->name);

         printf("%s sent out %s!\n", npc->name, fight_p.name);

      }

      else if (who >= 1 && who <= 4) {//npc

         printf("\e[2J\e[H");//clear map

         copy_data(&fight_p, &(npc->pokemon[0]));

         printf("%s would like to battle!\n", npc->name);

         printf("%s sent out %s!\n", npc->name, fight_p.name);

      }

      else {//wild

           //    printf("[2J[H");//clear map

           //

         printf("\e[2J\e[H");

         wild_rand = rand() % 10;

         copy_data(&fight_p, &Poke_Dataset[wild_rand]);

         gotoxy(30, 10);

         printf("wild %s appeared!\n", fight_p.name);

      }

      gotoxy(30, 11);

      printf("GO! %s!\n", my_p.name);

      sleep(2);

      //    printf("[2J[H");

      printf("\e[2J\e[H");

      while (my_p.current_hp > 0 ||

         fight_p.current_hp > 0) {

         show_pstate(&my_p, &fight_p);//show pokemon state

                               //what will pokemon do?

         printf("  What will %s do?\n", my_p.name);

         printf("  1. FIGHT\n");

         printf("  2. BAG\n");

         printf("  ▶  ");

         scanf("%d", &select1);

         if (select1 == 1) {//FIGHT

            printf("\n  Select skill\n");

            for (int i = 0; i < 4; i++) {

               printf(" %2d. %12s PP %d/15\n"

                  , i + 1, my_p.skill[i].name

                  , my_p.skill[i].pp);

            }

            printf("  ▶  ");

            while (1) {

               scanf("%d", &select_skill);

               if (my_p.skill[select_skill - 1].pp > 0) {

                  my_p.skill[select_skill - 1].pp--;

                  printf("Skill Used!!");

                  break;

               }

               else {

                  printf("this skill cannot be used!! pp is 0\nPlease enter again!");

               }

            }

            fight_p.current_hp -= my_p.attack;

            my_p.current_hp -= fight_p.attack;

            show_pstate(&my_p, &fight_p);

            printf("\e[2J\e[H");

            if (fight_p.current_hp <= 0) {

               gotoxy(30, 10);

               printf("Enemy %s fainted!\n", fight_p.name);

               gotoxy(30, 11);

               printf("%s gained Exp. Points!\n", my_p.name);

               if (my_p.level < 20)

                  LevelUp(&(my_p));

               sleep(3);

               if (who >= npc1 && who <= npc4) {

                  u->npc_fight[who - npc1] = 1;

               }

               else if (who == boss) {

                  show_ending();

               }

               copy_data(&u->pokemon[0], &my_p);

               return 1;

            }

            //used skill print

            else {

               printf("%s used %s!\n", my_p.name,

                  my_p.skill[select_skill - 1].name);

               random_skill = rand() % 4;

               printf("%s used %s!\n", fight_p.name,

                  fight_p.skill[random_skill].name);

               //my_p.current_hp -= fight_p.attack;//from t

               u->pokemon[0].current_hp -= fight_p.attack;

               show_pstate(&u->pokemon[0], &fight_p);

               if (u->pokemon[0].current_hp <= 0) {//to t

                  printf("\e[2J\e[H");

                  gotoxy(30, 10);

                  printf("%s blacked out!\n", my_p.name);

                  sleep(3);

                  if (strcmp(u->pokemon[1].name, "nothing")) {

                     copy_data(&u->pokemon[0], &u->pokemon[1]);

                     for (int i = 1; i< 3; i++)

                     {

                        copy_data(&u->pokemon[i], &u->pokemon[i + 1]);

                     }

                     strcpy(u->pokemon[3].name, "nothing");

                  }

                  else

                     Game();

                  return 0;

               }

            }

         }

         else if (select1 == 2) {//BAG

            show_inven(u);

            printf("  ▶  ");

            scanf("%d", &inum);

            printf("\e[2J\e[H");

            switch (inum) {

            case 1:

               if (u->item[medicine] > 0) {

                  printf("You used Medicine!\n");

                  u->item[medicine]--;

                  my_p.current_hp = my_p.max_hp;

               }

               else {

                  printf("You can't use Medicine\n");

               }

               break;

            case 2:

               if (u->item[m_ball] > 0) {

                  if (who >= 0 && who <= 4)

                     printf("The trainer blocked the BALL!\nDon't be a thief!\n");

                  else if (p_catch(u, &fight_p) == 1)

                     return 1;

               }

               else {

                  printf("You can't use Moster Ball\n");

               }

               break;

            case 3:

               if (u->item[pp_max] > 0) {

                  printf("You used PP Max!\n");

                  u->item[pp_max]--;

                  printf("Select Skill(1~4):");

                  scanf("%d", &snum);

                  my_p.skill[snum - 1].pp = max_pp;

               }

               else {

                  printf("You can't use PP Max\n");

               }

               break;

            }

         }

      }

   }

}

void show_inven(User* u) {

   printf("\n  Select item\n");

   printf("  1. Medicine     X %d\n", u->item[medicine]);

   printf("  2. Monster Ball X %d\n", u->item[m_ball]);

   printf("  3. PP max       X %d\n", u->item[pp_max]);

}

void show_plist(User* u) {

   gotoxy(23, 24);

   printf("POKEMONS\n");

   for (int i = 0; i<4; i++) {

      gotoxy(23, 25 + i);

      if (u->pokemon[i].current_hp != 0) {

         printf("%10s ", u->pokemon[i].name);

         printf("Lv%2d ", (int)u->pokemon[i].level);

         printf("%3d/", u->pokemon[i].current_hp);

         printf("%3d\n", u->pokemon[i].max_hp);

      }

   }

   int e;

   gotoxy(23, 29);

   printf("Retrun to menu(0):");

   while (1) {

      scanf("%d", &e);

      if (e == 0) {

         return;

      }

      else {

         printf("\b ");

      }

   }

}

void Center(User* u) {

   for (int i = 0; i < max_poke; i++) {

      u->pokemon[i].current_hp = u->pokemon[i].max_hp;

      for (int j = 0; j < 4; j++) {

         u->pokemon[i].skill[j].pp = max_pp;

      }

   }

}

void LevelUp(Pokemon* p) {

   if (p->level < 20) {

      int level = p->level + 1;

      p->level = level;

      p->attack = lv_attack[level - 1];

      p->max_hp = lv_hp[level - 1];

      p->current_hp = p->max_hp;

      if (level > 4) {

         level -= 3;

         Poke_Dataset[0].level = level;

         for (int i = 0; i < 10; i++)

         {

            Poke_Dataset[i].level++;

            Poke_Dataset[i].attack = lv_attack[level - 1];

            Poke_Dataset[i].max_hp = lv_hp[level - 1];

            Poke_Dataset[i].current_hp = lv_hp[level - 1];

         }

      }

   }

}

void show_ending() {

   printf("\e[2J\e[H");//clear

   printf("      TTTTTT H  H EEEE     EEEE N   N DDD\n");

   sleep(1);

   printf("   TT   H  H E        E    NN  N D  D\n");

   sleep(1);

   printf("   TT   HHHH EEE      EEE  N N N D  D\n");

   sleep(1);

   printf("   TT   H  H E        E    N  NN D  D\n");

   sleep(1);

   printf("   TT   H  H EEEE     EEEE N   N DDD\n\n");

   sleep(1);

 

   printf(">==>    >=> >=======> >=>     >=> >===========>        >==>\n");

   printf(">> >=>  >=> >=>        >=>   >=>       >=>                >=>\n");

   printf(">=> >=> >=> >=>         >=> >=>        >=>                  >=>\n");

   printf(">=>  >=>>=> >=====>       >=>          >=>                >=>\n");

   printf(">=>   > >=> >=>         >=> >=>        >=>              >=>\n");

   printf(">=>    >>=> >=>        >=>   >=>       >=>\n");

   printf(">=>     >=> >=======> >=>     >=>      >=>     >=> >=>  >=>\n");

   sleep(1);

   exit(1);

 

}

