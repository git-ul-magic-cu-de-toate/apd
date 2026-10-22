/* Implement this class. */

import java.util.concurrent.PriorityBlockingQueue;

public class MyHost extends Host {
    private final PriorityBlockingQueue<Task> myQueue = new PriorityBlockingQueue<>(100,
            (o1, o2) -> o2.getPriority() - o1.getPriority() == 0 ? o1.getStart() - o2.getStart() : o2.getPriority() - o1.getPriority());
    private volatile boolean isRunning = true;
    private final Object lock = new Object();
    private Task currentTask = null;

    @Override
    public void run() {
        while (isRunning) {
            synchronized (lock) {
                try {
                    while (myQueue.isEmpty() && isRunning) {
                        lock.wait();
                    }
                    if (!isRunning) {
                        break;
                    }

                    Task nextTask = myQueue.peek();
                    if (shouldPreempt(nextTask)) {
                        currentTask = myQueue.poll();
                    }
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    continue;
                }
            }

            processCurrentTask();
        }
    }

    private boolean shouldPreempt(Task nextTask) {
        return currentTask == null ||
                (currentTask.isPreemptible() && nextTask != null && nextTask.getPriority() > currentTask.getPriority());
    }

    private void processCurrentTask() {
        if (currentTask != null) {
            try {
                sleep(currentTask.getDuration());
                currentTask.finish();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            } finally {
                currentTask = null;
            }
        }
    }

    @Override
    public void addTask(Task task) {
        synchronized (lock) {
            myQueue.offer(task);
            lock.notify();
        }
    }

    @Override
    public int getQueueSize() {
        synchronized (lock) {
            return myQueue.size();
        }
    }

    @Override
    public long getWorkLeft() {
        synchronized (lock) {
            return myQueue.stream().mapToLong(Task::getDuration).sum();
        }
    }

    @Override
    public void shutdown() {
        synchronized (lock) {
            isRunning = false;
            lock.notifyAll();
        }
    }
}
