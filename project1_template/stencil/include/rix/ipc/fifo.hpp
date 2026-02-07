#pragma once

#include "rix/ipc/file.hpp"

namespace rix {
namespace ipc {


class Fifo : public File {
   public:
    enum class Mode : int {
        WRITE,
        READ
    };

    /**
     * @brief Creates a Fifo object by opening a named pipe specified by the 
     * pathname. This will create the fifo special file if it does not exist.
     * The fifo special file will be opened for both reading and writing.
     *  
     * @param pathname The path to the fifo special file
     * @param mode The mode to open the Fifo with (READ or WRITE)
     * @param nonblocking Flag to toggle non-blocking IO
     */
    Fifo(const std::string &pathname, Mode mode, bool nonblocking=false);

    /**
     * @brief Default constructor. This does not open a file descriptor.
     * 
     */
    Fifo();

    /**
     * @brief Copy constructor. This will duplicate the underlying file 
     * descriptors using `dup`.
     * 
     * @param src The Fifo to be copied
     */
    Fifo(const Fifo &src);

    /**
     * @brief Assignment operator. This will duplicate the underlying file 
     * descriptors using `dup`.
     * 
     * @param src The Fifo to be copied
     */
    Fifo &operator=(const Fifo &src);
    
    /**
     * @brief Move constructor. Moves the source file descriptor to the 
     * destination Fifo and invalidates the source Fifo.
     * 
     * @param src The Fifo to be moved
     */
    Fifo(Fifo &&src);

    /**
     * @brief Move assignment operator. If the destination Fifo is valid, close
     * the destination. Moves the source file descriptor to the destination Fifo
     * and invalidates the source Fifo.
     * 
     * @param src The Fifo to be moved
     */
    Fifo &operator=(Fifo &&src);

    /**
     * @brief Destructor. This will close the underlying file descriptor.
     * 
     */
    ~Fifo();

    /**
     * @brief Returns the pathname of the Fifo.
     * 
     */
    std::string pathname() const;

    /**
     * @brief Returns the mode of the Fifo
     * 
     */
    Mode mode() const;

   private:
    Mode mode_;
    std::string pathname_;
};

}  // namespace ipc
}  // namespace rix