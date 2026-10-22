/* Implement this class. */

import java.util.List;
import java.lang.*;
import java.util.concurrent.atomic.AtomicInteger;

public class MyDispatcher extends Dispatcher {
    private final AtomicInteger nextIndex = new AtomicInteger(0);
    private final List<Host> hosts;
    public MyDispatcher(SchedulingAlgorithm algorithm, List<Host> hosts) {
        super(algorithm, hosts);
        this.hosts = hosts;
    }
    @Override
    public void addTask(Task task) {
        if (algorithm == SchedulingAlgorithm.ROUND_ROBIN) {
                int index = nextIndex.getAndIncrement() % hosts.size();
                hosts.get(index).addTask(task);

        } else if (algorithm == SchedulingAlgorithm.SHORTEST_QUEUE) {
            int min = Integer.MAX_VALUE;
            int idx = 0;
            for (int i = 0; i < hosts.size(); i++){
                int size = hosts.get(i).getQueueSize();
                if (size < min) {
                    min = size;
                    idx = i;
                }
            }
            hosts.get(idx).addTask(task);

        } else if(algorithm == SchedulingAlgorithm.SIZE_INTERVAL_TASK_ASSIGNMENT) {
                int index = switch (task.getType()) {
                    case SHORT -> 0;
                    case MEDIUM -> 1;
                    case LONG -> 2;
                };
                hosts.get(index).addTask(task);

        } else if (algorithm == SchedulingAlgorithm.LEAST_WORK_LEFT) {
            long min = Long.MAX_VALUE;
            int idx = 0;
            for (int i = 0; i < hosts.size(); i++){
                long workLeft = hosts.get(i).getWorkLeft();
                if (workLeft < min) {
                    min = workLeft;
                    idx = i;
                }
            }
            hosts.get(idx).addTask(task);
        }
    }
}