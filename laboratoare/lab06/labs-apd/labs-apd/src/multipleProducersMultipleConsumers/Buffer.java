package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	private final int size = 100;
	private final ArrayBlockingQueue<Integer> buffer = new ArrayBlockingQueue<>(size);

	void put(int value) {
		try {
			buffer.put(value);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	int get() {
		int result = -1;
		try {
			result = buffer.take();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return result;
	}
}
