#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>


typedef struct{
    int rows ;
    int cols ;
    int**  element;
} Matrix ;

typedef struct{
    int rows ;
    int cols ;
} Index ;

Matrix matrix_a ;
Matrix matrix_b ;
Matrix matrix_c_per_matrix ;
Matrix matrix_c_per_row ;
Matrix matrix_c_per_element ;

void construct (Matrix* matrix ,int rows , int cols){
    matrix->rows = rows ;
    matrix->cols = cols ;
    matrix->element = (int**) malloc(sizeof(int*)*rows);
    for (int i = 0; i < rows; i++)
    {
        matrix->element[i] =(int*) malloc(sizeof(int)*cols);   
    }
    
}


void* read_file (char *filename ,Matrix* matrix){
    
    FILE *file  ;
    
    int rows , cols ;
    char filepath [] = "/home/hawas/Tech/OS/threads/test1/" ;
    strcat(filepath,filename);
    file = fopen(filepath,"r");
    if (file == NULL) {
        printf("Error: could not open file %s\n", filepath);
    }
   
        int i = 0 , tmp ;
        if(fscanf(file,"row=%d col=%d", &rows, &cols)){
            
            construct(matrix,rows,cols);
            
            for (int i = 0; i < matrix->rows; i++)
            {
                for (int j = 0; j < matrix->cols; j++)
                {
                    fscanf(file, "%d ",&tmp );
                    matrix->element[i][j] = tmp ;
                }
            }
        }
    
    fclose(file);   
}


// Print matrix in file 
void write_file(Matrix* matrix, char* filename)
{
    char str[255] = "/home/hawas/Tech/OS/threads/output/" ;
    strcat(str, filename);
    FILE* fd = fopen(str, "w");
    if (fd == NULL){
        printf("File %s Is Not Found\n", str);
        return;
    }
    int row = matrix->rows, col = matrix->cols;
    fprintf(fd,"row=%d  col=%d \n",row ,col);
    for(int i = 0; i < row; i++) {
            for(int j = 0; j < col; j++) {
                fprintf(fd, "%d ", matrix->element[i][j]);
            }
            fprintf(fd, "\n");
        }

    fclose(fd);
}

void print(Matrix* matrix)
{
    printf("row = %d col = %d\n", matrix->rows, matrix->cols);
    for(int i = 0; i < matrix->rows; i++) {
        for(int j = 0; j < matrix->cols; j++) {
            printf("%d ", matrix->element[i][j]);

        }
        printf("\n");
    }
    printf("\n");
}


void* mult_per_matrix(){
    //matrix_a(m*n)
    //matrix_b(n*p)
    int m =  matrix_a.rows ;
    int n =  matrix_a.cols ;
    int p =  matrix_b.cols ;
    
    int i, j, k, sum;
    for (i = 0; i < m; i++) {
        for (j = 0; j < p; j++) {
            sum = 0 ;
            for (k = 0; k < n; k++) {
                sum += matrix_a.element[i][k] * matrix_b.element[k][j];   
            }
            matrix_c_per_matrix.element[i][j] = sum ;
        }
    }
    
}


void* mult_per_row(void* arg){
   
    //matrix_a(m*n)
    //matrix_b(n*p)
    int m =  matrix_a.rows ;
    int n =  matrix_a.cols ;
    int p =  matrix_b.cols ;
    int i =  *(int* )arg ;
    int  j, k, sum;
    
        for (j = 0; j < p; j++) {
            sum = 0 ;
            for (k = 0; k < n; k++) {
                sum += matrix_a.element[i][k] * matrix_b.element[k][j];   
            }
            matrix_c_per_row.element[i][j] = sum ;
        }


}


void* mult_per_element(void* arg){
    //matrix_a(m*n)
    //matrix_b(n*p)
    
    int n =  matrix_a.cols ;
    int i = ((Index*)arg)->rows ;
    int j = ((Index*)arg)->cols ;
    int k, sum;
    
        
    sum = 0 ;
    for (k = 0; k < n; k++) {
        sum += matrix_a.element[i][k] * matrix_b.element[k][j];   
    }
    matrix_c_per_element.element[i][j] = sum ;
    
}

int main(){
    int threadCounter ; 
    // char a [10];
    // char b [10];
    // printf("Input:\n");
    // scanf("%s",a);
    // scanf("%s",b);
    // read_file(a,&matrix_a);
    // read_file(b,&matrix_b);
    read_file("a.txt",&matrix_a);
    read_file("b.txt",&matrix_b);
    construct(&matrix_c_per_matrix,matrix_a.rows,matrix_b.cols);
    construct(&matrix_c_per_row,matrix_a.rows,matrix_b.cols);
    construct(&matrix_c_per_element,matrix_a.rows,matrix_b.cols);  

    pthread_t   matrix_thread, 
                rowThread[matrix_a.rows],
                elementThread[matrix_a.rows][matrix_b.cols] ; 
    struct timeval stop, start;

    // Methode 1 (Thread per matrix)
    gettimeofday(&start, NULL); //start checking time
    
        pthread_create(&matrix_thread,NULL,&mult_per_matrix,NULL);
        pthread_join(matrix_thread ,NULL);

    gettimeofday(&stop, NULL); //end checking time
    printf("Thread per matrix create 1 thread and take: %lu microseconds\n", stop.tv_usec - start.tv_usec);



    // Methode 2 (Thread per row)
    int* row ;
    gettimeofday(&start, NULL); //start checking time

    for (int i = 0; i < matrix_a.rows; i++)
    {    
        row = (int*) malloc(sizeof(int));
        *row = i;
        pthread_create(&rowThread[i],NULL,&mult_per_row,row);
        threadCounter++;
    }
        
    for (int i = 0; i < matrix_a.rows; i++)
    {
        pthread_join(rowThread[i],NULL);
    }

    gettimeofday(&stop, NULL); //end checking time
    printf("Thread per row create %d threads and take: %lu microseconds\n",threadCounter, stop.tv_usec - start.tv_usec);



    // Methode 3 (Thread per element)
    Index* index ;
    threadCounter = 0 ;
    gettimeofday(&start, NULL); //start checking time
    for (int i = 0; i < matrix_a.rows; i++)
    {
        for (int j = 0; j < matrix_b.cols; j++)
        {
            index = (Index*) malloc(sizeof(int));
            index->rows = i ;
            index->cols = j;
            pthread_create(&elementThread[i][j], NULL, &mult_per_element, index);
            threadCounter++ ;
        }
        
    }


    for (int i = 0; i < matrix_a.rows; i++)
    {
        for (int j = 0; j < matrix_b.cols; j++)
        {            
            pthread_join(elementThread[i][j],NULL);
        }       
    }
    gettimeofday(&stop, NULL); //end checking time
    printf("Thread per element create %d threads and take: %lu microseconds\n",threadCounter, stop.tv_usec - start.tv_usec);
   
    //write in files
    write_file(&matrix_c_per_matrix,"C_Per_Matrix.txt");
    write_file(&matrix_c_per_row,"C_Per_Row.txt");
    write_file(&matrix_c_per_element,"C_Per_Element.txt");
    
    return 0 ;
}