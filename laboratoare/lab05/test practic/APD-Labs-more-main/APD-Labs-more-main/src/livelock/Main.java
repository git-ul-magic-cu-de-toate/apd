package livelock;

//Livelock describes situation where two threads are busy responding to actions of each other.
public class Main {
    static final LivelockFirstPart firstlock = new LivelockFirstPart();
 
    static final LivelockSecondPart secondlock = new LivelockSecondPart();

    public static void main(String[] args) {
 
 
        Thread t1 = new Thread(new Runnable() {
            public void run() {
                firstlock.firstLockRun(secondlock);
            }
        });
        t1.start();
 
        Thread t2 = new Thread(new Runnable() {
            public void run() {
                secondlock.secondLockRun(firstlock);
            }
        });
        t2.start();
    }
 
}
