/**
 ******************************************************************************
 * @file    queue.c
 * @author  MCD Application Team
 * @brief   Queue management
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright c 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "queue.h"
#include "utilities.h"
#include <stdint.h>
#include <string.h>

/* Private define ------------------------------------------------------------*/
/* Private typedef
 * -------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define MOD(X, Y) (((X) >= (Y)) ? ((X) - (Y)) : (X))

/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

int CircularQueue_Init(queue_t *q, uint8_t *queueBuffer, uint32_t queueSize,
                       uint16_t elementSize, uint8_t optionFlags) {
  q->qBuff = queueBuffer;
  q->first = 0;
  q->last = 0; // queueSize-1;
  q->byteCount = 0;
  q->elementCount = 0;
  q->queueMaxSize = queueSize;
  q->elementSize = elementSize;
  q->optionFlags = optionFlags;

  if ((optionFlags & CIRCULAR_QUEUE_SPLIT_IF_WRAPPING_FLAG) && q->elementSize) {
    /* can not deal with splitting at the end of buffer with fixed size element
     */
    return -1;
  }
  return 0;
}

uint8_t *CircularQueue_Add(queue_t *q, uint8_t *x, uint16_t elementSize,
                           uint32_t nbElements) {

  uint8_t *ptr = NULL; /* fct return ptr to the element freshly added, if no
                          room fct return NULL */
  uint16_t curElementSize =
      0; /* the size of the element currently  stored at q->last position */
  uint8_t elemSizeStorageRoom =
      0; /* Indicate the header (which contain only size) of element in case of
            varaibale size elemenet (q->elementsize == 0) */
  uint32_t curBuffPosition; /* the current position in the queue buffer */
  uint32_t i;               /* loop counter */
  uint32_t NbBytesToCopy = 0,
           NbCopiedBytes = 0; /* Indicators for copying bytes in queue */
  uint32_t eob_free_size;     /* Eof End of Quque Buffer Free Size */
  // uint32_t free_size;                             /* Total free size in the
  // queue (before adding elements */
  uint8_t wrap_will_occur = 0;      /* indicate if a wrap around will occurs */
  uint8_t wrapped_element_eob_size; /* In case of Wrap around, indicat size of
                                       parta of elemenet that fit at thened of
                                       the queuue  buffer */
  uint16_t overhead = 0; /* In case of CIRCULAR_QUEUE_SPLIT_IF_WRAPPING_FLAG or
                            CIRCULAR_QUEUE_NO_WRAP_FLAG options, indcate the
                            size overhead that will be generated by adding the
                            element with wrap management (split or no wrap ) */

  elemSizeStorageRoom = (q->elementSize == 0) ? 2 : 0;
  /* retrieve the size of last element sored: the value stored at the beginning
   * of the queue element if element size is variable otherwise take it from
   * fixed element Size member */
  if (q->byteCount) {
    curElementSize =
        (q->elementSize == 0)
            ? q->qBuff[q->last] +
                  ((q->qBuff[MOD((q->last + 1), q->queueMaxSize)]) << 8) + 2
            : q->elementSize;
  }
  /* if queue element have fixed size , reset the elementSize arg with fixed
   * element size value */
  if (q->elementSize > 0) {
    elementSize = q->elementSize;
  }

  eob_free_size =
      (q->last >= q->first) ? q->queueMaxSize - (q->last + curElementSize) : 0;
  // free_size = (q->last >= q->first) ?  eob_free_size + q->first :(q->last +
  // curElementSize) - q->first;

  /* check how many bytes of wrapped element (if anay) are at end of buffer */
  wrapped_element_eob_size =
      (((elementSize + elemSizeStorageRoom) * nbElements) < eob_free_size)
          ? 0
          : (eob_free_size % (elementSize + elemSizeStorageRoom));
  wrap_will_occur = wrapped_element_eob_size > elemSizeStorageRoom;

  overhead = (wrap_will_occur && (q->optionFlags & CIRCULAR_QUEUE_NO_WRAP_FLAG))
                 ? wrapped_element_eob_size
                 : overhead;
  overhead = (wrap_will_occur &&
              (q->optionFlags & CIRCULAR_QUEUE_SPLIT_IF_WRAPPING_FLAG))
                 ? elemSizeStorageRoom
                 : overhead;

  /* Store now the elements if ennough room for all elements */
  if (elementSize &&
      ((q->byteCount + ((elementSize + elemSizeStorageRoom) * nbElements) +
        overhead) <= q->queueMaxSize)) {
    /* loop to add all elements  */
    for (i = 0; i < nbElements; i++) {
      q->last = MOD((q->last + curElementSize), q->queueMaxSize);
      curBuffPosition = q->last;

      /* store the element  */
      /* store fisrt the element size if element size is varaible */
      if (q->elementSize == 0) {
        q->qBuff[curBuffPosition++] = elementSize & 0xFF;
        curBuffPosition = MOD(curBuffPosition, q->queueMaxSize);
        q->qBuff[curBuffPosition++] = (elementSize & 0xFF00) >> 8;
        curBuffPosition = MOD(curBuffPosition, q->queueMaxSize);
        q->byteCount += 2;
      }

      /* Identify number of bytes of copy takeing account possible wrap, in this
       * case NbBytesToCopy will contains size that fit at end of the queue
       * buffer */
      NbBytesToCopy = MIN((q->queueMaxSize - curBuffPosition), elementSize);
      /* check if no wrap (NbBytesToCopy == elementSize) or if Wrap and no
         spsicf option; In thi case part of data will copied at the end of the
         buffer and the rest a the beggining */
      if ((NbBytesToCopy == elementSize) ||
          ((NbBytesToCopy < elementSize) &&
           (q->optionFlags == CIRCULAR_QUEUE_NO_FLAG))) {
        /* Copy First part (or emtire buffer ) from current position up to the
         * end of the buffer queue (or before if enough room)  */
        memcpy(&q->qBuff[curBuffPosition], &x[i * elementSize], NbBytesToCopy);
        /* Adjust bytes count */
        q->byteCount += NbBytesToCopy;
        /* Wrap */
        curBuffPosition = 0;
        /* set NbCopiedBytes bytes with  ampount copied */
        NbCopiedBytes = NbBytesToCopy;
        /* set the rest to copy if wrao , if no wrap will be 0 */
        NbBytesToCopy = elementSize - NbBytesToCopy;
        /* set the current element Size, will be used to calaculate next last
         * position at beggining of loop */
        curElementSize = (elementSize) + elemSizeStorageRoom;
      } else if (NbBytesToCopy) /* We have a wrap  to manage */
      {
        /* case of CIRCULAR_QUEUE_NO_WRAP_FLAG option */
        if (q->optionFlags & CIRCULAR_QUEUE_NO_WRAP_FLAG) {
          /* if element size are variable and NO_WRAP option, Invalidate end of
           * buffer setting 0xFFFF size*/
          if (q->elementSize == 0) {
            q->qBuff[curBuffPosition - 2] = 0xFF;
            q->qBuff[curBuffPosition - 1] = 0xFF;
          }
          q->byteCount +=
              NbBytesToCopy; /* invalid data at the end of buffer are take into
                                account in byteCount */
          /* No bytes coped a the end of buffer */
          NbCopiedBytes = 0;
          /* all element to be copied at the begnning of buffer */
          NbBytesToCopy = elementSize;
          /* Wrap */
          curBuffPosition = 0;
          /* if variable size element, invalidate end of buffer setting OxFFFF
           * in element header (size) */
          if (q->elementSize == 0) {
            q->qBuff[curBuffPosition++] = NbBytesToCopy & 0xFF;
            q->qBuff[curBuffPosition++] = (NbBytesToCopy & 0xFF00) >> 8;
            q->byteCount += 2;
          }

        }
        /* case of CIRCULAR_QUEUE_SPLIT_IF_WRAPPING_FLAG option */
        else if (q->optionFlags & CIRCULAR_QUEUE_SPLIT_IF_WRAPPING_FLAG) {
          if (q->elementSize == 0) {
            /* reset the size of current element to the nb bytes fitting at the
             * end of buffer */
            q->qBuff[curBuffPosition - 2] = NbBytesToCopy & 0xFF;
            q->qBuff[curBuffPosition - 1] = (NbBytesToCopy & 0xFF00) >> 8;
            /* copy the bytes */
            memcpy(&q->qBuff[curBuffPosition], &x[i * elementSize],
                   NbBytesToCopy);
            q->byteCount += NbBytesToCopy;
            /* set the number of copied bytes */
            NbCopiedBytes = NbBytesToCopy;
            /* set rest of data to be copied to begnning of buffer */
            NbBytesToCopy = elementSize - NbBytesToCopy;
            /* one element more dur to split in 2 elements */
            q->elementCount++;
            /* Wrap */
            curBuffPosition = 0;
            /* Set new size for rest of data */
            q->qBuff[curBuffPosition++] = NbBytesToCopy & 0xFF;
            q->qBuff[curBuffPosition++] = (NbBytesToCopy & 0xFF00) >> 8;
            q->byteCount += 2;
          } else {
            /* Should not occur */
            /* can not manage split Flag on Fixed size element */
            /* Buffer is corrupted */
            return NULL;
          }
        }
        curElementSize = (NbBytesToCopy) + elemSizeStorageRoom;
        q->last = 0;
      }

      /* some remaning byte to copy */
      if (NbBytesToCopy) {
        memcpy(&q->qBuff[curBuffPosition],
               &x[(i * elementSize) + NbCopiedBytes], NbBytesToCopy);
        q->byteCount += NbBytesToCopy;
      }

      /* One more element */
      q->elementCount++;
    }

    // q->elementCount+=nbElements;
    // q->byteCount+=((elementSize + elemSizeStorageRoom )*nbElements);

    ptr = q->qBuff + (MOD((q->last + elemSizeStorageRoom), q->queueMaxSize));
  }
  /* for Breakpoint only...to remove */
  else {
    return NULL;
  }
  return ptr;
}

uint8_t *CircularQueue_Remove(queue_t *q, uint16_t *elementSize) {
  uint8_t elemSizeStorageRoom = 0;
  uint8_t *ptr = NULL;
  elemSizeStorageRoom = (q->elementSize == 0) ? 2 : 0;
  *elementSize = 0;
  // uint32_t FirstElemetPos = 0;
  if (q->byteCount > 0) {
    /* retreive element Size */
    *elementSize =
        (q->elementSize == 0)
            ? q->qBuff[q->first] +
                  ((q->qBuff[MOD((q->first + 1), q->queueMaxSize)]) << 8)
            : q->elementSize;

    if ((q->optionFlags & CIRCULAR_QUEUE_NO_WRAP_FLAG) &&
        !(q->optionFlags & CIRCULAR_QUEUE_SPLIT_IF_WRAPPING_FLAG)) {
      if (((*elementSize == 0xFFFF) && q->elementSize == 0) ||
          ((q->first > q->last) && q->elementSize &&
           ((q->queueMaxSize - q->first) < q->elementSize))) {
        /* all data from current position up to the end of buffer are invalid */
        q->byteCount -= (q->queueMaxSize - q->first);
        /* Adjust first element pos */
        q->first = 0;
        /* retrieve the rigth size after the wrap [if varaible size element] */
        *elementSize =
            (q->elementSize == 0)
                ? q->qBuff[q->first] +
                      ((q->qBuff[MOD((q->first + 1), q->queueMaxSize)]) << 8)
                : q->elementSize;
      }
    }

    /* retreive element */
    ptr = q->qBuff + (MOD((q->first + elemSizeStorageRoom), q->queueMaxSize));

    /* adjust byte count */
    q->byteCount -= (*elementSize + elemSizeStorageRoom);

    /* Adjust q->first */
    if (q->byteCount > 0) {
      q->first =
          MOD((q->first + *elementSize + elemSizeStorageRoom), q->queueMaxSize);
    }
    /* adjust element count */
    --q->elementCount;
  }
  return ptr;
}

uint8_t *CircularQueue_Sense(queue_t *q, uint16_t *elementSize) {
  uint8_t elemSizeStorageRoom = 0;
  uint8_t *x = NULL;
  elemSizeStorageRoom = (q->elementSize == 0) ? 2 : 0;
  *elementSize = 0;
  uint32_t FirstElemetPos = 0;

  if (q->byteCount > 0) {
    FirstElemetPos = q->first;
    *elementSize =
        (q->elementSize == 0)
            ? q->qBuff[q->first] +
                  ((q->qBuff[MOD((q->first + 1), q->queueMaxSize)]) << 8)
            : q->elementSize;

    if ((q->optionFlags & CIRCULAR_QUEUE_NO_WRAP_FLAG) &&
        !(q->optionFlags & CIRCULAR_QUEUE_SPLIT_IF_WRAPPING_FLAG)) {
      if (((*elementSize == 0xFFFF) && q->elementSize == 0) ||
          ((q->first > q->last) && q->elementSize &&
           ((q->queueMaxSize - q->first) < q->elementSize)))

      {
        /* all data from current position up to the end of buffer are invalid */
        FirstElemetPos = 0; /* wrap to the begiining of buffer */

        /* retrieve the rigth size after the wrap [if varaible size element] */
        *elementSize =
            (q->elementSize == 0)
                ? q->qBuff[FirstElemetPos] +
                      ((q->qBuff[MOD((FirstElemetPos + 1), q->queueMaxSize)])
                       << 8)
                : q->elementSize;
      }
    }
    /* retrieve element */
    x = q->qBuff +
        (MOD((FirstElemetPos + elemSizeStorageRoom), q->queueMaxSize));
  }
  return x;
}

int CircularQueue_Empty(queue_t *q) {
  int ret = 0;
  if (q->byteCount <= 0) {
    ret = 1;
  }
  return ret;
}

int CircularQueue_NbElement(queue_t *q) { return q->elementCount; }
