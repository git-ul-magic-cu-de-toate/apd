import java.util.concurrent.PriorityBlockingQueue;

public class File {
    public static void main(String[] args) throws InterruptedException {
        int P = Integer.parseInt(args[0]);
        Thread[] threads = new Thread[P];
        int[] sleepDurations = new int[P - 1];
        PriorityBlockingQueue<Job> queue = new PriorityBlockingQueue<>();

        // Citirea duratelor de sleep pentru ultimele P-1 thread-uri
        for (int i = 1; i < P; i++) {
            sleepDurations[i - 1] = Integer.parseInt(args[i]);
        }

        // Crearea thread-urilor
        for (int i = 1; i < P; i++) {
            final int sleepDuration = sleepDurations[i - 1];
            final int threadId = i;
            threads[i] = new Thread(() -> {
                try {
                    Thread.sleep(sleepDuration * 1000);
                    queue.add(new Job(threadId));
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            });
        }

        // Crearea și pornirea thread-ului 0
        threads[0] = new Thread(() -> {
            for (int j = 1; j < P; j++) {
                try {
                    Job job = queue.take();
                    System.out.println("Thread-ul " + job.threadId + " a terminat");
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        });

        // Pornirea thread-urilor
        for (int i = 0; i < P; i++) {
            threads[i].start();
        }

        // Așteptarea terminării execuției thread-urilor
        for (int i = 0; i < P; i++) {
            threads[i].join();
        }
    }

    static class Job implements Comparable<Job> {
        int threadId;

        Job(int threadId) {
            this.threadId = threadId;
        }

        @Override
        public int compareTo(Job o) {
            return Integer.compare(this.threadId, o.threadId);
        }
    }
}
