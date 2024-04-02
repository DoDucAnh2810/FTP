public class SequenceTableau<Loulou> implements Sequence<Loulou> {
	Object [] elements;
	int debut, taille;

	public SequenceTableau() {
		elements = new Object[1];
	}

	private void redimensionne() {
		if (taille >= elements.length) {
			Object [] nouveau = new Object[elements.length*2];
			int fin = Math.min(debut+taille, elements.length);
			for (int i=debut; i<fin; i++)
				nouveau[i] = elements[i];
			fin = (debut+taille)-elements.length;
			for (int i=0; i<fin; i++) {
				nouveau[i+elements.length] = elements[i];
			}
			elements = nouveau;
		}
	}

	public void insereTete(Loulou element) {
		redimensionne();
		debut--;
		if (debut < 0)
			debut += elements.length;
		elements[debut] = element;
		taille++;
	}

	public void insereQueue(Loulou element) {
		redimensionne();
		int position = (debut+taille)%elements.length;
		elements[position] = element;
		taille++;
	}

	@SuppressWarnings("unchecked")
	public Loulou extraitTete() {
		if (taille == 0)
			throw new RuntimeException("Sequence vide");
		Loulou resultat = (Loulou) elements[debut];
		taille--;
		debut = (debut+1)%elements.length;
		return resultat;
	}

	public boolean estVide() {
		return taille == 0;
	}

	@Override
	public Iterateur<Loulou> iterateur() {
		return new IterateurTableau<>(this);
	}

	public String toString() {
		String resultat = "Sequence tableau [ ";
		int fin = Math.min(debut+taille, elements.length);
		for (int i=debut; i<fin; i++)
			resultat += elements[i] + " ";
		fin = (debut+taille)-elements.length;
		for (int i=0; i<fin; i++)
			resultat += elements[i] + " ";
		resultat += "]";
		return resultat;
	}
}
