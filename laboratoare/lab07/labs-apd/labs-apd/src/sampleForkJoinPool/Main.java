package sampleForkJoinPool;

import java.util.concurrent.ForkJoinPool;

public class Main {

	public static void main(String[] args) {
		ForkJoinPool tpe = new ForkJoinPool(4);
		Task task = new Task("files");
		tpe.invoke(task);
		tpe.shutdown();
	}
}
