/*
 * This file is part of see-object.
 *
 * see-object is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * see-object is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with see-object.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef SEE_RANDOM_HPP
#define SEE_RANDOM_HPP

#include <random>

class Random {

    public:

        Random()
            : m_seed(0)
        {
            std::random_device rd;
            seed_rand(std::random_device()());
        }

        void seed_rand(unsigned seed)
        {
            m_seed = seed;
            gen.seed(m_seed);
        }

        unsigned get_seed() const
        {
            return m_seed;
        }

        /**
         * \brief return a value from the range [min, max].
         */
        unsigned uniform_uint32_range(unsigned min, unsigned max)
        {
            std::uniform_int_distribution<unsigned>dist(min, max);
            return dist(gen);
        }

        /**
         * \brief return a random 32bit integer
         */
        unsigned uniform_uint32()
        {
            constexpr unsigned min = std::numeric_limits<unsigned>::min();
            constexpr unsigned max = std::numeric_limits<unsigned>::max();
            return uniform_uint32_range(min, max);
        }

        /**
         * \brief return a value from the range [min, max].
         */
        int uniform_int32_range(int min, int max)
        {
            std::uniform_int_distribution<int>dist(min, max);
            return dist(gen);
        }

        /**
         * \brief return a random 32bit integer
         * @return
         */
        int uniform_int32()
        {
            constexpr auto min = std::numeric_limits<int>::min();
            constexpr auto max = std::numeric_limits<int>::max();
            return uniform_uint32_range(min, max);
        }

        /**
         * \brief generate floats in the range [min, max]
         */
        double uniform_float_range(double min, double max)
        {
            std::uniform_real_distribution<double>dist(min, max);
            return dist(gen);
        }

        /**
         * \brief return a number in the range[0.0, 1.0]
         * @return value between and including 0.0  and 1.0
         */
        double uniform_float()
        {
            return uniform_float_range(0.0, 1.0);
        }

        /**
         * \brief return a number from the range [0.0, 1.0)
         * \note in contrast to uniform_float, this will not generate 1.0
         */
        double canonical_float()
        {
            return std::generate_canonical<
                double,
                std::numeric_limits<double>::digits
                >(gen);
        }

        /**
         * \brief Draw a number from a normal distribution with a mean and
         *        standard deviation.
         */
        double normal_float(double mean, double std)
        {
            std::normal_distribution<double> dist{mean, std};
            return dist(gen);
        }

    private:

        std::mt19937 gen;

        unsigned m_seed;
};

#endif //ifndef SEE_RANDOM_HPP