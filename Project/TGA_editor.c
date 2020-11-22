#include "main.h"

void matrix_init(Matrix* matrix, int rows, int cols)
{
    matrix->rows = rows;
    matrix->cols = cols;

    matrix->size = rows * cols;                   

    matrix->data = (RGBcolor*)malloc(sizeof(RGBcolor) * matrix->size);

    return;
}
void load_tga(TGAHeader* header, Matrix** image, const char* filename)
{
    FILE* tgaFile = fopen(filename, "rb");

    if(tgaFile != NULL)
    {
        fread(&header->idlength, sizeof(header->idlength), 1, tgaFile);
        fread(&header->colourmaptype, sizeof(header->colourmaptype), 1, tgaFile);
        fread(&header->datatypecode, sizeof(header->datatypecode), 1, tgaFile);
        fread(&header->colourmaporigin, sizeof(header->colourmaporigin), 1, tgaFile);
        fread(&header->colourmaplength, sizeof(header->colourmaplength), 1, tgaFile);
        fread(&header->colourmapdepth, sizeof(header->colourmapdepth), 1, tgaFile);
        fread(&header->x_origin, sizeof(header->x_origin), 1, tgaFile);
        fread(&header->y_origin, sizeof(header->y_origin), 1, tgaFile);
        fread(&header->width, sizeof(header->width), 1, tgaFile);
        fread(&header->height, sizeof(header->height), 1, tgaFile);
        fread(&header->bitsperpixel, sizeof(header->bitsperpixel), 1, tgaFile);
        fread(&header->imagedescriptor, sizeof(header->imagedescriptor), 1, tgaFile);

        *image = (Matrix*)malloc(sizeof(Matrix));
        matrix_init(*image, header->height,header->width);

        int imageSize = header->width * header->height;

        for(int i = 0; i < imageSize;i++)
        {
            fread(&(*image)->data[i].B, sizeof(uint8_t), 1, tgaFile);
            fread(&(*image)->data[i].G, sizeof(uint8_t), 1, tgaFile);
            fread(&(*image)->data[i].R, sizeof(uint8_t), 1, tgaFile);
        }
        fclose(tgaFile);
    }
    else
    {
        printf("\nVstupní obrázek nenalezen!\n\n");
        exit(1);
    }
    
    return;
}
void save_tga(TGAHeader header, Matrix image, const char* filename)
{
    FILE* tgaFile = fopen(filename, "wb");

    if(tgaFile != NULL)
    {
        fwrite(&header.idlength, sizeof(header.idlength), 1, tgaFile);
        fwrite(&header.colourmaptype, sizeof(header.colourmaptype), 1, tgaFile);
        fwrite(&header.datatypecode, sizeof(header.datatypecode), 1, tgaFile);
        fwrite(&header.colourmaporigin, sizeof(header.colourmaporigin), 1, tgaFile);
        fwrite(&header.colourmaplength, sizeof(header.colourmaplength), 1, tgaFile);
        fwrite(&header.colourmapdepth, sizeof(header.colourmapdepth), 1, tgaFile);
        fwrite(&header.x_origin, sizeof(header.x_origin), 1, tgaFile);
        fwrite(&header.y_origin, sizeof(header.y_origin), 1, tgaFile);
        fwrite(&header.width, sizeof(header.width), 1, tgaFile);
        fwrite(&header.height, sizeof(header.height), 1, tgaFile);
        fwrite(&header.bitsperpixel, sizeof(header.bitsperpixel), 1, tgaFile);
        fwrite(&header.imagedescriptor, sizeof(header.imagedescriptor), 1, tgaFile);

        for(int i = 0;i < image.size; i++)
        {
            fwrite(&image.data[i].B, sizeof(image.data[i].B), 1 , tgaFile);
            fwrite(&image.data[i].G, sizeof(image.data[i].G), 1 , tgaFile);
            fwrite(&image.data[i].R, sizeof(image.data[i].R), 1 , tgaFile);
        }
        fclose(tgaFile); 
    }

    return;
}
void resize(TGAHeader *header, Matrix* image, int width, int height)
{
    Matrix* helpImage = (Matrix*)malloc(sizeof(Matrix));
    matrix_init(helpImage, height, width);

    double xRatio = image->cols/(double)width;
    double yRatio = image->rows/(double)height;

    double px, py;

    for(int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            px = floor(j*xRatio);
            py = floor(i*yRatio);

            helpImage->data[(i * width) + j].R = image->data[(int)((py*image->cols) + px)].R;
            helpImage->data[(i * width) + j].G = image->data[(int)((py*image->cols) + px)].G;
            helpImage->data[(i * width) + j].B = image->data[(int)((py*image->cols) + px)].B;
        }
    }

    header->width = width;
    header->height = height;
    image->cols = width;
    image->rows = height;
    image->size = width * height;

     for (int i = 0; i < image->size; i++)
    {
        image->data[i].R = helpImage->data[i].R;
        image->data[i].G = helpImage->data[i].G;
        image->data[i].B = helpImage->data[i].B;
    }

    free(helpImage);
    helpImage = NULL;

    return;
}
void crop(TGAHeader* header, Matrix* image, int x_position, int y_position, int width, int height)
{
    int position = (x_position * header->width + y_position);

    Matrix* helpImage = (Matrix*)malloc(sizeof(Matrix));
    matrix_init(helpImage, height, width);
    int k = 0;

    for(int i = 0; i < height; i++)
    {
        for(int j = position; j < (position + width); j++)
        {
            helpImage->data[k].R = image->data[j].R;
            helpImage->data[k].G = image->data[j].G;
            helpImage->data[k].B = image->data[j].B;

            k++;
        }
        position = position + (header->width);
    }

    header->width = width;
    header->height = height;
    image->cols = width;
    image->rows = height;
    image->size = width * height;

    for (int i = 0; i < image->size; i++)
    {
        image->data[i].R = helpImage->data[i].R;
        image->data[i].G = helpImage->data[i].G;
        image->data[i].B = helpImage->data[i].B;
    }

    free(helpImage);
    helpImage = NULL;

    return;
}
void arguments_check(int argc)
{
    if(argc < 5)
    {
        printf("\nDo programu musí vstupovat minimálně 2 argumenty(vstup, vystup), další argumenty jsou nepovinné!\n\nVzorové spuštění: ./projekt -i vstup -o vystup -crop x,y,w,h -resize w,h\n\n");
        exit(1);
    }
}
char argument_check(char* argument)
{
    int counter = 0;

    for(int i = 0; argument[i] != '\0'; i++)
    {
        if(argument[i] == ',')
        {
            counter++;
        }
    }

    switch (counter)                    //kontrola, zda byla zadana funkce crop nebo resize
    {
        case 1:
            return 'r';
            break;
        case 3:
            return 'c';
            break;
        default:
            return '!';
            break;
    }
}
void arguments_positive_check(char* argument)
{
    for(int i = 0; i < strlen(argument); i++)
    {
        if(argument[i] == '-')
        {
            printf("\nChybně zadané parametry. Souřadnice musí být číslo!\n\n");
            exit(1);
        }
    }

    return;
}
int get_y_crop(char* argument)
{
    int N = strlen(argument);
    char number[N];
    int i;
    int j;

    for(i = 0; argument[i] != ','; i++)
    {
        number[i] = argument[i];        //kopirovani znaku před čarkou
    }

    for(j = 0; j < i; j++)
    {
        if(!isdigit(number[j])) break;  //kontrola, zda jsou znaky číslo
    }
    
    int y = i == j ? atoi(number) : -1;

    if(y != -1)
    {
        return y;
    }
    else
    {
        printf("\nChybně zadané parametry. Souřadnice musí být číslo!\n\n");
        exit(1);
    } 
}
int get_x_crop(char* argument)
{
    int N = strlen(argument);
    char number[N];
    int i;
    int j;
    int k;
    int c = 0;

    for(i = 0; argument[i] != ','; i++);
    
    for(j = (i+1); argument[j] != ','; j++)
    {
        number[c] = argument[j];
        c++;
    }

    for(k = 0;k < c;k++)
    {
        if(!isdigit(number[k]))
        {
            break;   //kontrola, zda jsou znaky číslo
        }  
    }

    int x = c == k ? atoi(number) : -1;

    if(x != -1)
    {
        return x;
    }
    else
    {
        printf("\nChybně zadané parametry. Souřadnice musí být číslo!\n\n");
        exit(1);
    } 
}
int get_w_crop(char* argument)
{
    int N = strlen(argument);
    char number[N];
    int i;
    int j;
    int c = 0;
    int k;
    int t;

    for(t = 0; argument[t] != ','; t++);

    for(i = (t+1); argument[i] != ','; i++);

    for(j = (i+1); argument[j] != ','; j++)
    {
        number[c] = argument[j];
        c++;
    }

    for(k = 0;k < c; k++)
    {
        if(!isdigit(number[k]))
        {
            break;   //kontrola, zda jsou znaky číslo
        }  
    }

    int w = c == k ? atoi(number) : -1;

    if(w != -1)
    {
        return w;
    }
    else
    {
        printf("\nChybně zadané parametry. Souřadnice musí být číslo!\n\n");
        exit(1);
    } 
}
int get_h_crop(char* argument)
{   
    int N = strlen(argument);
    char number[N];
    int t;
    int i;
    int k;
    int p;
    int j;
    int o;
    int c = 0;

    for(t = 0; argument[t] != ','; t++);

    for(i = (t+1); argument[i] != ','; i++);

    for(k = (i+1); argument[k] != ','; k++);

    for(j = (k+1); j < N; j++)
    {
        number[c] = argument[j];
        c++;
    }

    for(o = 0; o < c; o++)
    {
        if(!isdigit(number[o]))
        {
            break;   //kontrola, zda jsou znaky číslo
        }  
    }

   int h = c == o ? atoi(number) : 0;

    if(h != 0)
    {
        return h;
    }
    else
    {
        printf("\nChybně zadané parametry. Souřadnice musí být číslo!\n\n");
        exit(1);
    } 
}
int get_h_resize(char* argument)
{

    int N = strlen(argument);
    char number[N];
    int comma_position;
    int index_of_number;
    int length_of_number = 0;
    int is_digit_counter;

    for(comma_position = 0; argument[comma_position] != ','; comma_position++);           //spocita pozici carky

    for(index_of_number = (comma_position+1); index_of_number< N; index_of_number++)
    {
        number[length_of_number] = argument[index_of_number];               //kopirovani cisla
        length_of_number++;
    }

    for(is_digit_counter = 0;is_digit_counter <length_of_number;is_digit_counter++)
    {
        if(!isdigit(number[is_digit_counter]))
        {
            break;   //kontrola, zda jsou znaky číslo
        }  
    }
    
    int h = length_of_number == is_digit_counter ? atoi(number) : -1;        //kontrola, zda jsou znaky číslo

    if(h != -1)
    {
        return h;
    }
    else
    {
        printf("\nChybně zadané parametry. Souřadnice musí být číslo!\n\n");
        exit(1);
    } 
}
int get_w_resize(char* argument)
{
    int N = strlen(argument);
    char number[N];
    int i;
    int j;

    for(i = 0; argument[i] != ','; i++)
    {
        number[i] = argument[i];        //kopirovani znaku před čarkou
    }

    for(j = 0; j < i; j++)
    {
        if(!isdigit(number[j])) break;  //kontrola, zda jsou znaky číslo
    }
    
    int w = i == j ? atoi(number) : 0;

    if(w != 0)
    {
        return w;
    }
    else
    {
        printf("\nChybně zadané parametry. Souřadnice musí být číslo!\n");
        exit(1);
    } 
}
void crop_check(TGAHeader* header, int x, int y, int w, int h)
{
    if(!(h <= (header->height - y)) || !(w <= (header->width - x)) || !(h <= header->height) || !(w <= header->width))
    {
        printf("Výřez zasahuje mimo obrázek, nebo je větší, než obrázek.\n\n");
        exit(1);
    }
}
int main(int argc, char *argv[])
{   
    arguments_check(argc);

    bool edited = false;
    int opArgCount = (argc - 5) / 2;
    int editCount = 0;
    char* command;
    char* input;
    char* output;
    char* arguments[opArgCount];
    
    for(int i = 1; argv[i] != NULL; i++)
    {
       command = argv[i];
        if(command[0] == '-')
        {
            if(command[1] == 'i' && i == 1)
            {
                i++;
                input=argv[i];
            }
            if(command[1] == 'o' && i == 3)
            {
                i++;
                output=argv[i];
            }
            if(command[1] == 'c' && command[2] == 'r' && command[3] == 'o' && command[4] == 'p')
            {
                i++;
                arguments[editCount] = argv[i];
                editCount++;
            }
            if(command[1] == 'r' && command[2] == 'e' && command[3] == 's' && command[4] == 'i' && command[5] == 'z' && command[6] == 'e')
            {
                i++;
                arguments[editCount]= argv[i];
                editCount++;
            }  
        }
        else
            {
                printf("\nChybně zadané argumenty!\n\nVzorové spuštění: ./projekt -i vstup -o vystup -crop x,y,w,h -resize w,h\n\n");
                exit(1);
            }
    }

    TGAHeader loadedHeader;
    Matrix* loadedImage = NULL;

    load_tga(&loadedHeader, &loadedImage, input);

    printf("\nRozměry načteného obrázku: %dx%d\n\n",loadedHeader.width, loadedHeader.height);

    for(int i = 0; i < opArgCount; i++)
    {
        if(argument_check(arguments[i]) == 'c')
        {
            arguments_positive_check(arguments[i]);
            crop_check(&loadedHeader, get_x_crop(arguments[i]), get_y_crop(arguments[i]), get_w_crop(arguments[i]), get_h_crop(arguments[i]));
            crop(&loadedHeader, loadedImage, get_x_crop(arguments[i]), get_y_crop(arguments[i]), get_w_crop(arguments[i]), get_h_crop(arguments[i]));
            edited = true;
        }
        else if(argument_check(arguments[i]) == 'r')
        {
            arguments_positive_check(arguments[i]);
            resize(&loadedHeader, loadedImage, get_w_resize(arguments[i]), get_h_resize(arguments[i]));
            edited = true;
        }
        else
        {
            printf("\nChybně zadané argumenty!\n\nVzorové spuštění: ./projekt -i vstup -o vystup -crop x,y,w,h -resize w,h\n\n");
            exit(1);
        }
    }
    
    if(edited)
    {
        printf("\nRozměry upraveného obrázku: %dx%d\n\n",loadedHeader.width, loadedHeader.height);
    }

    save_tga(loadedHeader, *loadedImage, output);

    return 0;
}