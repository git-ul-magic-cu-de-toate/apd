#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRACKER_RANK 0
#define MAX_FILES 10
#define MAX_FILENAME 15
#define HASH_SIZE 32
#define MAX_CHUNKS 100

#define TAG_INIT 0
#define TAG_SWARM_REQ 1
#define TAG_SEG_REQ 2
#define TAG_UPDATE 3
#define TAG_DONE 4


typedef struct {
    int owned; //1=downloaded 0=to download
    char hash[HASH_SIZE];
}Segment;


typedef struct Swarm_unit {
    int client;
    int* segments_pos;
}Swarm_unit;

typedef struct {
    char filename[MAX_FILENAME];
    int nr_segments;
    int nr_segments_owned;
    Segment* segments;
    int swarm_index;
    Swarm_unit* swarm;
}FileData;

//shared data for threads
int nr_files, nr_files_to_download;
FileData* client_files;

void freeFileData(FileData *files, int count) {

    for (int i = 0; i < count; i++) {
        if(files[i].nr_segments_owned!=0)
            {free(files[i].segments);}
        if (files[i].swarm != NULL) {   
            for (int j = 0; j < files[i].swarm_index; j++) {
                
                free(files[i].swarm[j].segments_pos);
            }
        }
        free(files[i].swarm);
    }
    free(files);
}


int select_seed_for_segment(int file_index, int segment_index)
{
    int valid_seeds[client_files[file_index].swarm_index];
    int valid_count = 0;
    // Identify all seeds that have the segment
    for (int i = 0; i < client_files[file_index].swarm_index; i++) { 
        if (client_files[file_index].swarm[i].segments_pos[segment_index] == 1) {
            valid_seeds[valid_count++] = client_files[file_index].swarm[i].client;
        }
    }

    // If no valid seed is found, return -1
    if (valid_count == 0) {
        return -1;
    }

    // Select a random seed from the valid seeds
    int random_index = rand() % valid_count;
    return valid_seeds[random_index];
}

int find_missing_segment(int file_index) {

    for (int i = 0; i < client_files[file_index].nr_segments; i++) {
        if (client_files[file_index].segments[i].owned == 0) { // status 0 means not downloaded
            return i; // Return index of the first missing segment
        }
    }

    return -1; // All segments are downloaded or no segments in the file
}


void request_swarm(int rank) {
    MPI_Status status;
    for(int i = nr_files; i<nr_files+nr_files_to_download;i++)
    {
        // Send swarm request for the file i
        MPI_Send(client_files[i].filename, MAX_FILENAME, MPI_CHAR, 0, TAG_SWARM_REQ, MPI_COMM_WORLD);
        MPI_Recv(&client_files[i].swarm_index, 1, MPI_INT, 0, TAG_SWARM_REQ, MPI_COMM_WORLD, &status);
        
        MPI_Recv(&client_files[i].nr_segments, 1, MPI_INT, 0, TAG_SWARM_REQ, MPI_COMM_WORLD, &status);

        // Allocate memory for segments and swarm
        client_files[i].segments = (Segment*)malloc(sizeof(Segment) * client_files[i].nr_segments);

        for(int j=0;j<client_files[i].nr_segments; j++)
        {
            client_files[i].segments[j].owned=0; //mark to download
        }
        client_files[i].nr_segments_owned=0;

        client_files[i].swarm = (Swarm_unit*)malloc(sizeof(Swarm_unit) * client_files[i].swarm_index);

        // Receive swarm details
        for (int j = 0; j < client_files[i].swarm_index; j++) {
            MPI_Recv(&client_files[i].swarm[j].client, 1, MPI_INT, 0, TAG_SWARM_REQ, MPI_COMM_WORLD, &status);
            
            client_files[i].swarm[j].segments_pos = (int*)malloc(sizeof(int) * client_files[i].nr_segments);
            MPI_Recv(client_files[i].swarm[j].segments_pos, client_files[i].nr_segments, MPI_INT, 0, TAG_SWARM_REQ, MPI_COMM_WORLD, &status);       
        }
    }
}

void request_swarm_update(int rank) {
    MPI_Status status;
    
    for(int i = nr_files; i<nr_files+nr_files_to_download;i++)
    {
        // Send swarm request for the file i     
        int swarm_index;
        MPI_Send(client_files[i].filename, MAX_FILENAME, MPI_CHAR, 0, TAG_SWARM_REQ, MPI_COMM_WORLD);
        MPI_Recv(&swarm_index, 1, MPI_INT, 0, TAG_SWARM_REQ, MPI_COMM_WORLD, &status);
        MPI_Recv(&client_files[i].nr_segments, 1, MPI_INT, 0, TAG_SWARM_REQ, MPI_COMM_WORLD, &status);

        //check if new seed in swarm
        if(client_files[i].swarm_index< swarm_index){
            client_files[i].swarm_index=swarm_index;
            client_files[i].swarm=(Swarm_unit*)realloc(client_files[i].swarm, sizeof(Swarm_unit)*swarm_index);
            client_files[i].swarm[swarm_index-1].segments_pos = (int*)malloc(sizeof(int) * client_files[i].nr_segments);
        }
   
        // Receive swarm details
        for (int j = 0; j < client_files[i].swarm_index; j++) {
            int aux[client_files[i].nr_segments];
            MPI_Recv(&client_files[i].swarm[j].client, 1, MPI_INT, 0, TAG_SWARM_REQ, MPI_COMM_WORLD, &status);
            
            MPI_Recv(aux, client_files[i].nr_segments, MPI_INT, 0, TAG_SWARM_REQ, MPI_COMM_WORLD, &status);       
            
            free(client_files[i].swarm[j].segments_pos);
            client_files[i].swarm[j].segments_pos=(int*)calloc(client_files[i].nr_segments +1, sizeof(int));
            
            for(int p = 0; p < client_files[i].nr_segments; p++)
            {
                client_files[i].swarm[j].segments_pos[p]=aux[p];
            }  
        }
    }
    
}

void request_segment_from_seed(int seed_rank, int file_index, int segment_index)
{
    MPI_Status status;
    MPI_Send(client_files[file_index].filename, MAX_FILENAME, MPI_CHAR, seed_rank, TAG_SEG_REQ, MPI_COMM_WORLD); //from witch file
    MPI_Send(&segment_index, 1, MPI_INT, seed_rank, TAG_SEG_REQ, MPI_COMM_WORLD);
    MPI_Recv(client_files[file_index].segments[segment_index].hash, HASH_SIZE, MPI_CHAR, seed_rank, 7, MPI_COMM_WORLD, &status);
    
    client_files[file_index].segments[segment_index].owned=1; 
    client_files[file_index].nr_segments_owned++;

}

void send_data_to_tracker(int tag,int nr_f)
{
    //send update data
    MPI_Send(&nr_f, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    for (int i = 0; i<nr_f; i++)
    {
        MPI_Send(client_files[i].filename, MAX_FILENAME, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
        MPI_Send(&client_files[i].nr_segments_owned, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
        for (int j = 0; j< client_files[i].nr_segments_owned; j ++)
        {
            MPI_Send(client_files[i].segments[j].hash, HASH_SIZE, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
        }

    }
}

void write_downloaded_file(int rank, int file_index) {

    char output_filename[MAX_FILENAME + 10]; // Extra space for rank and space
    sprintf(output_filename, "client%d_%s", rank, client_files[file_index].filename); // Creating the output file name
    FILE* out_file = fopen(output_filename, "w");
    if (out_file == NULL) {
        printf("Error opening output file: %s\n", output_filename);
        return;
    }

    for (int i = 0; i < client_files[file_index].nr_segments; i++) {
        fwrite(client_files[file_index].segments[i].hash,sizeof(char),HASH_SIZE,out_file);
        fwrite("\n", sizeof(char), 1, out_file);
    }
    fclose(out_file);
}

void *download_thread_func(void *arg)
{
    MPI_Status status;
    char ack[3];
    int rank = *(int*) arg;

    ////////////////////////INITIALIZATION////////////////////////
    
    send_data_to_tracker(TAG_INIT, nr_files);

    // wait for ack from tracker
    MPI_Recv(ack,3,MPI_CHAR,0,0,MPI_COMM_WORLD,&status);

    // ////////////////////////DOWNLOAD////////////////////////  
    request_swarm(rank);

    char update[MAX_FILENAME];
    strcpy(update,"update");
    
    while(nr_files<nr_files+nr_files_to_download){
        int count = 10; //after 10 segments

        while(count)
        {
            //search first segment missing
            int segment_index=-1; 
            
            segment_index = find_missing_segment(nr_files);
            if(segment_index ==-1) //file downloaded completely
            {   
                // write file
                write_downloaded_file(rank,nr_files);
                nr_files++;
                nr_files_to_download--;
                if(nr_files==nr_files+nr_files_to_download)
                    break;
            }
            else {
                int seed=select_seed_for_segment(nr_files,segment_index); // random seed from swarm
                request_segment_from_seed(seed,nr_files,segment_index);
                count--; 
            }
        }


        //send updated data to tracker
        MPI_Send(update, MAX_FILENAME, MPI_CHAR, 0,TAG_UPDATE, MPI_COMM_WORLD);

        int nr_f=nr_files+1;
        if (nr_files==nr_files+nr_files_to_download)
            nr_f--;
        send_data_to_tracker(TAG_UPDATE,nr_f);
        MPI_Recv(ack,3,MPI_CHAR,0,TAG_UPDATE,MPI_COMM_WORLD,&status);
        
        // request_swarm_update(rank);   
    }

    char done[MAX_FILENAME];
    strcpy(done,"I'm done");
    MPI_Send(done, MAX_FILENAME, MPI_CHAR, 0, TAG_DONE, MPI_COMM_WORLD);

    return NULL;
}

void *upload_thread_func(void *arg)
{
    int rank = *(int*) arg;
    MPI_Status status;
    while (1) {
        // Receive a request
        char request_filename[MAX_FILENAME];
        int request_segment_index;

        MPI_Recv(request_filename, MAX_FILENAME, MPI_CHAR, MPI_ANY_SOURCE, TAG_SEG_REQ, MPI_COMM_WORLD, &status);
        if (status.MPI_SOURCE==0 && strcmp(request_filename,"Stop")==0) //Stop message from tracker
        {
            break;
        }
        int aux=status.MPI_SOURCE;
        MPI_Recv(&request_segment_index, 1, MPI_INT, aux, TAG_SEG_REQ, MPI_COMM_WORLD, &status);
        for (int i = 0; i < nr_files; i++) {
            if (strcmp(client_files[i].filename, request_filename) == 0) {
                MPI_Send(client_files[i].segments[request_segment_index].hash, HASH_SIZE, MPI_CHAR, aux, 7, MPI_COMM_WORLD);
                break;
            }
        }
    } 

    return NULL;
}

void send_swarm(int sender_rank, FileData* files,int total_nr_files,char* requested_file){

    for (int i = 0; i < total_nr_files; i++) {
        if (strcmp(files[i].filename, requested_file) == 0) {
            // File found, send the swarm data
            int num_peers = files[i].swarm_index;
            int nr_segments= files[i].nr_segments;
            MPI_Send(&num_peers, 1, MPI_INT, sender_rank, TAG_SWARM_REQ, MPI_COMM_WORLD);
            MPI_Send(&nr_segments, 1, MPI_INT, sender_rank, TAG_SWARM_REQ, MPI_COMM_WORLD);
         
            for (int j = 0; j < num_peers; j++) {
                // Send details of each peer in the swarm
                int peer_id = files[i].swarm[j].client;
                MPI_Send(&peer_id, 1, MPI_INT, sender_rank, TAG_SWARM_REQ, MPI_COMM_WORLD);
                MPI_Send(files[i].swarm[j].segments_pos, nr_segments, MPI_INT, sender_rank, TAG_SWARM_REQ, MPI_COMM_WORLD);
                
            }
            break;  // Exit the loop once the requested file's swarm is sent
        }
    }
    
}

void tracker(int numtasks, int rank) {

    MPI_Status status;
    int total_nr_files=0;
    FileData files[MAX_FILES];
    int nr_recive;
    int new_file,nr_ffrom_client;

    ////////////////////////INITIALIZATION////////////////////////

    for(int i = 1; i<numtasks;i++)
    {
        MPI_Recv(&nr_ffrom_client,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
        while(nr_ffrom_client>0){
            char filename_recv[MAX_FILENAME], hash_recive[HASH_SIZE];
            new_file=1;
        
            MPI_Recv(filename_recv,MAX_FILENAME,MPI_CHAR,i,0,MPI_COMM_WORLD,&status);
            for(int j = 0; j <total_nr_files; j++)
            {
                //tracker already has this file and we just update the swarm
                if(strcmp(filename_recv,files[j].filename)==0)
                {
                    new_file=0;
                    files[j].swarm[files[j].swarm_index].client=i;
                    MPI_Recv(&nr_recive,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);

                    for(int l=0;l<nr_recive;l++)
                    {
                        MPI_Recv(hash_recive,HASH_SIZE,MPI_CHAR,i,0,MPI_COMM_WORLD,&status);
                        files[j].swarm[files[j].swarm_index].segments_pos[l]=1; //mark that client has the segment
                    }
                    
                    files[j].swarm_index++;

                }
            }
           
            //file is new
            if(new_file==1)
            {
                //we save the new file and alloc space
                strcpy(files[total_nr_files].filename,filename_recv);
                files[total_nr_files].swarm_index=0;
                files[total_nr_files].swarm=(Swarm_unit*)malloc(sizeof(Swarm_unit)*numtasks);
                files[total_nr_files].swarm[0].client=i;

                //recive number of segments
                MPI_Recv(&nr_recive,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
                  
                files[total_nr_files].nr_segments=nr_recive;
                files[total_nr_files].nr_segments_owned=nr_recive;
                files[total_nr_files].segments=(Segment*)malloc(sizeof(Segment)*files[total_nr_files].nr_segments);
                
                for(int j=0;j<numtasks;j++)
                {
                    files[total_nr_files].swarm[j].segments_pos = calloc(files[total_nr_files].nr_segments, sizeof(int));
                }
                
                for(int j=0;j<nr_recive;j++) //nr_Recive
                {
                    MPI_Recv(files[total_nr_files].segments[j].hash,HASH_SIZE,MPI_CHAR,i,0,MPI_COMM_WORLD,&status);
                    files[total_nr_files].segments[j].owned=1;
                    files[total_nr_files].swarm[0].segments_pos[j]=1; //mark in swarm
                } 

                files[total_nr_files].swarm_index=1;
                total_nr_files++;
            }
            nr_ffrom_client--;
        }
    }

    for(int i = 1; i<numtasks;i++)
    {
        MPI_Send("ACK", 3, MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }

    ////////////////////////DOWNLOAD////////////////////////
    int received_tag, sender_rank; 
    int count=numtasks-1;
    while(count)
    {
        char data_recv[MAX_FILENAME];
        MPI_Recv(data_recv,MAX_FILENAME,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

        sender_rank = status.MPI_SOURCE;
        received_tag = status.MPI_TAG;

        switch (received_tag)
        {
        case TAG_SWARM_REQ:
            send_swarm(sender_rank, files,total_nr_files, data_recv);
            break;

        case TAG_UPDATE:
 
            MPI_Recv(&nr_ffrom_client,1,MPI_INT,sender_rank,TAG_UPDATE,MPI_COMM_WORLD,&status);
            while(nr_ffrom_client>0){
                char filename_recv[MAX_FILENAME], hash_recive[HASH_SIZE];
                MPI_Recv(filename_recv,MAX_FILENAME,MPI_CHAR,sender_rank,TAG_UPDATE,MPI_COMM_WORLD,&status);
                for(int j = 0; j <total_nr_files; j++)
                {
                    //find the file
                    if(strcmp(filename_recv,files[j].filename)==0)
                    {
                        int index_swarm=files[j].swarm_index;
                        for(int k=0;k<files[j].swarm_index;k++)
                        {
                            if(files[j].swarm[k].client==sender_rank)
                            {
                                index_swarm=k;
                                break;
                            }
                        }

                        files[j].swarm[index_swarm].client=sender_rank;
                        MPI_Recv(&nr_recive,1,MPI_INT,sender_rank,TAG_UPDATE,MPI_COMM_WORLD,&status);

                        for(int l=0;l<nr_recive;l++)
                        {
                            MPI_Recv(hash_recive,HASH_SIZE,MPI_CHAR,sender_rank,TAG_UPDATE,MPI_COMM_WORLD,&status);
                            files[j].swarm[index_swarm].segments_pos[l]=1; //mark that client has the segment
                        }
                        
                        if(index_swarm==files[j].swarm_index)
                            files[j].swarm_index++;

                        break;
                    }
                }
                nr_ffrom_client--;
            }
            
            MPI_Send("ACK", 3, MPI_CHAR, sender_rank,TAG_UPDATE, MPI_COMM_WORLD);
            break;    
        case TAG_DONE:
            count--;
            break;
        default:
            break;
        }
        
    }

    char stop[MAX_FILENAME];
    strcpy(stop,"Stop");

    for(int i=1;i<numtasks;i++)
    {
        MPI_Send(stop, MAX_FILENAME, MPI_CHAR, i, TAG_SEG_REQ, MPI_COMM_WORLD); //send stop message to all update threads
    }

}

void peer_data_init(int rank) {

    char filename[MAX_FILENAME];
    FILE* file;
    
    // Construct the filename based on the rank
    sprintf(filename, "in%d.txt", rank);
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Eroare la citirea fisierului");
        exit(1);
    }
    fscanf(file, "%d\n", &nr_files);

    if(nr_files>0)
        client_files=(FileData*)malloc(sizeof(FileData)*nr_files);
    
    for(int i = 0; i < nr_files; i++)
    {
        fscanf(file, "%s %d\n", client_files[i].filename, &client_files[i].nr_segments);
        client_files[i].nr_segments_owned=client_files[i].nr_segments;
        client_files[i].segments=(Segment*)malloc(sizeof(Segment)*client_files[i].nr_segments);
        client_files[i].swarm=NULL;

        for (int j = 0; j < client_files[i].nr_segments; j++) {
            fread(client_files[i].segments[j].hash, HASH_SIZE, sizeof(char),file);
            fgetc(file);
            client_files[i].segments[j].owned = 1; //mark as owned
        }     
    }

    fscanf(file, "%d", &nr_files_to_download);

    if(nr_files>0)
        {
            client_files = (FileData*)realloc(client_files, sizeof(FileData) * (nr_files + nr_files_to_download));
            
        }
    else
        client_files = (FileData*)malloc(sizeof(FileData)*nr_files_to_download);

    for (int i = nr_files; i < nr_files + nr_files_to_download; i++) {
        fscanf(file, "%s", client_files[i].filename);
    }

    fclose(file);
}

void peer(int numtasks, int rank) {
    
    pthread_t download_thread;
    pthread_t upload_thread;
    void *status;
    int r;
    
    peer_data_init(rank);

    r = pthread_create(&download_thread, NULL, download_thread_func, (void *) &rank);
    if (r) {
        printf("Eroare la crearea thread-ului de download\n");
        exit(-1);
    }

    r = pthread_create(&upload_thread, NULL, upload_thread_func, (void *) &rank);
    if (r) {
        printf("Eroare la crearea thread-ului de upload\n");
        exit(-1);
    }

    r = pthread_join(download_thread, &status);
    if (r) {
        printf("Eroare la asteptarea thread-ului de download\n");
        exit(-1);
    }

    r = pthread_join(upload_thread, &status);
    if (r) {
        printf("Eroare la asteptarea thread-ului de upload\n");
        exit(-1);
    }

    freeFileData(client_files,nr_files);
}
 
int main (int argc, char *argv[]) {
    int numtasks, rank;
 
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "MPI nu are suport pentru multi-threading\n");
        exit(-1);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == TRACKER_RANK) {
        tracker(numtasks, rank);
    } else {
        peer(numtasks, rank);
    }

    MPI_Finalize();
}
