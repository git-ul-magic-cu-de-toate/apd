package ClassThreadExtends;

public class Task extends Thread{
    private int id;

    Task(int id){
        this.id = id;
    }
    public void run() {
      //  System.out.println("variabila" + Main.a);
        System.out.println("Hello world from thread:" + id);
    }
}
