#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <string>
#include "../DataReader.hpp"
#include "Option.hpp"

/// \brief Classe OptionFactory abstraite
class OptionFactory
{
    /**
     * Créer l'objet option selon l'option type donné
     *
     * @param[in] data pointeur vers l'objet DataReader contenant tous les params du json
     * @return option
     */
    public:
        static Option* buildOption(DataReader *data);
};
