package readersWriters.writerPriority;

import java.util.concurrent.Semaphore;

public class ReaderWriterSharedVars {
    // The value to read/write
    volatile int shared_value;
    // TODO: Add semaphores and anything else needed for synchronization
    Semaphore no_readers_sem, no_writer_sem, read_count_mutex, write_count_mutex;
    int read_count, write_count;

    ReaderWriterSharedVars(int init_shared_value) {
        this.shared_value = init_shared_value;

        this.no_readers_sem = new Semaphore(1);
        this.no_writer_sem = new Semaphore(1);

        this.read_count_mutex = new Semaphore(1);
        this.write_count_mutex = new Semaphore(1);

        this.read_count = 0;
        this.write_count = 0;

    }

}
