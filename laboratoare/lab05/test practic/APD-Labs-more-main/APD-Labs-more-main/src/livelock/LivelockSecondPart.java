package livelock;

public class LivelockSecondPart {
    private boolean secondActionMade = false;
 
    public void secondLockRun(LivelockFirstPart firstlock) {
        while (!firstlock.firstAction()) {
 
            System.out.println("FIRST thread's waiting for SECOND thread's action");
 
            try {
                Thread.sleep(100);
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
 
        System.out.println("SECOND thread made action so FIRST do it's action");
 
        this.secondActionMade = true;
    }
 
    public boolean secondAction() {
        return this.secondActionMade;
    }
}

