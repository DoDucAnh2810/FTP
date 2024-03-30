public class Couple<Val, Prio extends Comparable<Prio>> implements Comparable<Couple<Val, Prio>> {
	Val v;
	Prio p;

	Couple(Val valeur, Prio priorite) {
		v = valeur;
		p = priorite;
	}

	@Override
	public int compareTo(Couple<Val, Prio> o) {
		return p.compareTo(o.p);
	}

	@Override
	public String toString() {
		return "(" + v + ", " + p + ")";
	}

	public boolean equals(Couple<Double, Integer> o) {
		return (v == o.v) && (p == o.p);
	}
}
