public abstract class FAP<Bob> {
	Sequence<Bob> s;

	abstract void insere(Bob element);
	Bob extrait() {
		return s.extraitTete();
	}
	boolean estVide() {
		return s.estVide();
	}
}
