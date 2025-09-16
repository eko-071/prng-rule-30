#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#define MAX 128

#ifdef _WIN32
#include <windows.h>

void sleep_screen(){
    Sleep(100);
}

void clear_screen(){
    system("cls");
}

#else

#include <unistd.h>

void sleep_screen(){
    usleep(100000);
}

void clear_screen(){
    system("clear");
}

#endif


void initialise_grid(int array[][MAX], int bits2){
    for(int i=0; i<bits2; i++){
        for(int j=0; j<bits2; j++){
            array[i][j] = 0;
        }
    }
}

int check_range(int start, int bits2, int row){
    if(start + bits2/2 < bits2){
        if(row >= start && row < start + bits2/2) return 1;
        return 0;
    }
    if(row >= start || row < (start + bits2/2)%bits2) return 1;
    return 0;
}

void print_grid(int grid[][MAX], int bits2, int start){
    for(int i=0; i<bits2; i++){
        for(int j=0; j<bits2; j++){
            //Prints a blue hash if cell is 1, and a gray hash if the cell is 0
            if(grid[i][j]==1 && j==bits2/2 && check_range(start, bits2, i)) printf("\033[31m1 \033[0m");
            else if(grid[i][j]==0 && j==bits2/2 && check_range(start, bits2, i)) printf("\033[31m0 \033[0m");
            else if(grid[i][j]==1) printf("\033[34m1 \033[0m");
            else printf("\033[90m0 \033[0m");
        }
        printf("\n");
    }
}

void create_next_iteration(int present[][MAX], int bits2, int row){
    for(int j=0; j<bits2; j++){
        int left = present[row][(j-1 + bits2)%bits2];
        int mid = present[row][j];
        int right = present[row][(j+1)%bits2];

        present[row+1][j] = left ^ mid ^ right;
    }
}

uint64_t get_number(int present[][MAX], int bits2, int start){
    int power = 0, bits = bits2/2;
    uint64_t num = 0;
    printf("Binary: ");
    for(int i=0; i<bits; i++){
        printf("%d", present[(i+start)%bits2][bits]);
        num += ((uint64_t)present[(i+start)%bits2][bits]) << (bits - power - 1);
        power++;
    }
    printf("\n");
    return num;
}

uint64_t run_ca(int bits2, long long int seed){
    static int grid_a[MAX][MAX];
    int (*present)[MAX] = grid_a;

    initialise_grid(present, bits2);
    if(seed == 0) seed = (time(NULL) << 30) + time(NULL);
    for(int i=0; i<bits2; i++){
        present[0][i] = seed & 1;
        seed = seed >> 1;
    }

    int start = time(NULL)%bits2;

    for(int i=0; i<bits2-1; i++){
        clear_screen();
        printf("Iteration %d\n", i);
        print_grid(present, bits2, start);
        printf("Iteration %d\n", i);
        create_next_iteration(present, bits2, i);
        sleep_screen();
    }

    clear_screen();
    printf("Iteration %d\n", bits2-1);
    print_grid(present, bits2, start);
    printf("Iteration %d\n", bits2-1);

    return get_number(present, bits2, start);
}

int main(){
    int bits;
    long long int seed = 0;
    printf("Enter bit size of the number needed: ");
    scanf("%d", &bits);
    getchar();

    if(bits<1 || bits>MAX/2){
        printf("Error: Bit size should be between 1 and %d\n", MAX/2);
        return 1;
    }

    char choice;
    printf("Do you wish to enter your own custom seed? (y/n): ");
    scanf("%c", &choice);
    if(choice == 'y'){
        printf("Enter seed: ");
        scanf("%lld", &seed);
    }

    uint64_t num = run_ca(bits*2, seed);
    printf("The random number generated is %"PRIu64".\n", num);
}