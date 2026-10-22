
public class Main {
    public static void main(String[] args) {
        Thread[] threads = new Thread[8];
            for (int i = 0; i < 8; i++) {
                threads[i] = new Thread(new MyThread(i));
                threads[i].start();
            }
            for (int i = 0; i < 8; i++) {
                try {
                    threads[i].join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
    }


}