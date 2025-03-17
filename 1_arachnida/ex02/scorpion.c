#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define EXIF_MARKER 0xFFE1
#define EXIF_HEADER "Exif\0\0"
#define EXIF_HEADER_SIZE 6

// Structure pour représenter un tag EXIF
typedef struct {
    uint16_t tag;
    uint16_t type;
    uint32_t count;
    uint32_t value_offset;
} ExifTag;

// Fonction pour convertir 2 octets en uint16_t selon l'ordre des octets
uint16_t get_uint16(const uint8_t *data, int is_little_endian) {
    if (is_little_endian)
        return data[0] | (data[1] << 8);
    else
        return (data[0] << 8) | data[1];
}

// Fonction pour convertir 4 octets en uint32_t selon l'ordre des octets
uint32_t get_uint32(const uint8_t *data, int is_little_endian) {
    if (is_little_endian)
        return data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    else
        return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

// Fonction pour analyser et afficher un tag EXIF
void print_exif_tag(const uint8_t *tiff_data, ExifTag *tag, int is_little_endian) {
    printf("Tag: 0x%04X, Type: %d, Count: %d, Value/Offset: 0x%08X\n", 
           tag->tag, tag->type, tag->count, tag->value_offset);
    
    // Ici vous pouvez ajouter la logique pour interpréter les valeurs spécifiques
    // selon le type et le tag ID
}

// Fonction principale pour analyser les données EXIF
void parse_exif(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Impossible d'ouvrir le fichier %s\n", filename);
        return;
    }
    
    uint8_t marker[2];
    uint8_t length_bytes[2];
    uint16_t length;
    
    // Vérifier le marqueur SOI (Start of Image) JPEG
    if (fread(marker, 1, 2, fp) != 2 || marker[0] != 0xFF || marker[1] != 0xD8) {
        printf("Pas un fichier JPEG valide\n");
        fclose(fp);
        return;
    }
    
    // Chercher le marqueur EXIF
    while (1) {
        if (fread(marker, 1, 2, fp) != 2 || marker[0] != 0xFF) {
            printf("Pas de données EXIF trouvées\n");
            fclose(fp);
            return;
        }
        
        // Si c'est le marqueur EXIF
        if (marker[1] == (EXIF_MARKER & 0xFF)) {
            break;
        }
        
        // Sinon, sauter ce segment
        if (fread(length_bytes, 1, 2, fp) != 2) {
            printf("Erreur lors de la lecture de la longueur du segment\n");
            fclose(fp);
            return;
        }
        
        length = (length_bytes[0] << 8) | length_bytes[1];
        fseek(fp, length - 2, SEEK_CUR);
    }
    
    // Lire la longueur du segment EXIF
    if (fread(length_bytes, 1, 2, fp) != 2) {
        printf("Erreur lors de la lecture de la longueur du segment EXIF\n");
        fclose(fp);
        return;
    }
    
    length = (length_bytes[0] << 8) | length_bytes[1];
    
    // Allouer la mémoire pour le segment EXIF
    uint8_t *exif_data = (uint8_t *)malloc(length - 2);
    if (!exif_data) {
        printf("Erreur d'allocation mémoire\n");
        fclose(fp);
        return;
    }
    
    // Lire les données EXIF
    if (fread(exif_data, 1, length - 2, fp) != length - 2) {
        printf("Erreur lors de la lecture des données EXIF\n");
        free(exif_data);
        fclose(fp);
        return;
    }
    
    // Vérifier l'en-tête EXIF
    if (memcmp(exif_data, EXIF_HEADER, EXIF_HEADER_SIZE) != 0) {
        printf("En-tête EXIF invalide\n");
        free(exif_data);
        fclose(fp);
        return;
    }
    
    // Les données TIFF commencent après l'en-tête EXIF
    uint8_t *tiff_data = exif_data + EXIF_HEADER_SIZE;
    
    // Déterminer l'ordre des octets
    int is_little_endian;
    if (tiff_data[0] == 'I' && tiff_data[1] == 'I')
        is_little_endian = 1;  // Intel (little-endian)
    else if (tiff_data[0] == 'M' && tiff_data[1] == 'M')
        is_little_endian = 0;  // Motorola (big-endian)
    else {
        printf("Format TIFF inconnu\n");
        free(exif_data);
        fclose(fp);
        return;
    }
    
    // Vérifier la valeur du marqueur TIFF (42)
    uint16_t tiff_marker = get_uint16(tiff_data + 2, is_little_endian);
    if (tiff_marker != 42) {
        printf("Marqueur TIFF invalide\n");
        free(exif_data);
        fclose(fp);
        return;
    }
    
    // Obtenir l'offset du premier IFD (Image File Directory)
    uint32_t ifd_offset = get_uint32(tiff_data + 4, is_little_endian);
    uint8_t *ifd_data = tiff_data + ifd_offset;
    
    // Lire le nombre d'entrées dans l'IFD
    uint16_t num_entries = get_uint16(ifd_data, is_little_endian);
    printf("Nombre d'entrées EXIF: %d\n", num_entries);
    
    // Analyser chaque entrée
    for (int i = 0; i < num_entries; i++) {
        ExifTag tag;
        uint8_t *entry_data = ifd_data + 2 + (i * 12);
        
        tag.tag = get_uint16(entry_data, is_little_endian);
        tag.type = get_uint16(entry_data + 2, is_little_endian);
        tag.count = get_uint32(entry_data + 4, is_little_endian);
        tag.value_offset = get_uint32(entry_data + 8, is_little_endian);
        
        print_exif_tag(tiff_data, &tag, is_little_endian);
    }
    
    free(exif_data);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <fichier_image>\n", argv[0]);
        return 1;
    }
    
    parse_exif(argv[1]);
    return 0;
}