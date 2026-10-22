package reentrantBarSem;

public class MyThread extends Thread {
    private final int id;
    private final ReentrantBarrierWithSemaphores barrier;

    public MyThread(int id, ReentrantBarrierWithSemaphores barrier) {
        this.id = id;
        this.barrier = barrier;
    }

    @Override
    public void run() {
        for (int i = 0; i < 4; i++) {
            System.out.println("Before barrier " + id);
            try {
                barrier.await();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("After barrier " + id);

            try {
                barrier.await();
            } catch (InterruptedException e) {
                e.printStackTrace();

            }
        }
    }
}
