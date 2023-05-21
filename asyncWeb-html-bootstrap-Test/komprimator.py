import os
import gzip

input_folder = "preData"
output_folder = "data"

# Vytvoříme výstupní složku pokud ještě neexistuje
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

# Procházíme všechny soubory včetně souborů v podsložkách vstupní složky
for root, dirs, files in os.walk(input_folder):
    for file in files:
        # Vstupní a výstupní cesty pro aktuální soubor
        input_path = os.path.join(root, file)
        output_path = os.path.join(output_folder, file + ".gz")

        # Komprimujeme soubor a uložíme ho do výstupní složky
        with open(input_path, "rb") as input_file, gzip.open(output_path, "wb") as output_file:
            output_file.write(input_file.read())
