package livelock;
public class LivelockFirstPart {
    private boolean firstActionMade = false;
 
    public void firstLockRun(LivelockSecondPart secondlock) {
 
        while (!secondlock.secondAction()) {
 
            System.out.println("SECOND thread's waiting for FIRST thread's action");
 
            try {
                Thread.sleep(100);
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
 
        System.out.println("FIRST thread made action so SECOND do its action");
 
        this.firstActionMade = true;
    }
 
    public boolean firstAction() {
        return this.firstActionMade;
    }
 
}
