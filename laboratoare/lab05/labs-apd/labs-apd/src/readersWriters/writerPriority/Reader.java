package readersWriters.writerPriority;

public class Reader extends Thread {
    private final int id;
    private final int number_of_reads;
    private final long wait_time_ms;
    private final ReaderWriterSharedVars shared_vars;
    private double start_time;
    private double completion_time;

    public Reader(int id, int number_of_reads, long wait_time_ms, ReaderWriterSharedVars shared_vars) {
        this.id = id;
        this.number_of_reads = number_of_reads;
        this.wait_time_ms = wait_time_ms;
        this.shared_vars = shared_vars;
    }

    @Override
    public void run() {
        this.start_time = System.currentTimeMillis() / 1000.0;
        try {
            for (int i = 0; i < number_of_reads; i++) {

                // Ma asigur ca nu e interzis pentru readers (adica e writer in zona sau li se da prioritate)
                shared_vars.no_readers_sem.acquire();

                // Daca sunt primul cititor, blochez scriitorii
                shared_vars.read_count_mutex.acquire();
                    shared_vars.read_count++;
                    if (shared_vars.read_count == 1) {
                        shared_vars.no_writer_sem.acquire();
                    }
                shared_vars.read_count_mutex.release();

                // Au voie si alti cititori sa intre
                shared_vars.no_readers_sem.release();

                // Citesc
                read();

                // Daca ies ultimul, dau voie scriitorilor sa intre
                shared_vars.read_count_mutex.acquire();
                    shared_vars.read_count--;
                    if (shared_vars.read_count == 0) {
                        shared_vars.no_writer_sem.release();
                    }
                shared_vars.read_count_mutex.release();
            }
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }

        this.completion_time = System.currentTimeMillis() / 1000.0 - this.start_time;

        System.out.println("Reader with ID = " + this.id + " ended after " + completion_time + "s");
    }

    public double getCompletion_time() {
        return completion_time;
    }

    public void read() {
        System.out.println(Utils.get_current_time_str() + " | Thread " + this.id + " reads " + shared_vars.shared_value);
        try {
            sleep(wait_time_ms);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }
}
