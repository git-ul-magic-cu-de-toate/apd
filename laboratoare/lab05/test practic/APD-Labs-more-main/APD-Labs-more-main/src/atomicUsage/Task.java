package atomicUsage;


public class Task extends Thread{
    private int id;

    Task(int id){
        this.id = id;
    }
    public void run() {
        for(int i = 0; i < 10000; i++) {
            Main.myInteger.addAndGet(1);
        }
    }
}