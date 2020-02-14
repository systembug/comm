#pragma once
#include <array>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace SPLidar {
	class ViewerPointCloud {
	public:
		/*
		ViewerPointCloud()
			: m_x(""), m_y(""), m_z(""), m_intensity(""), m_index(0)
		{}
		ViewerPointCloud(float x, float y, float z, float intensity, int32_t index)
			: m_x(std::to_string(x)), m_y(std::to_string(y)), m_z(std::to_string(z)),
			m_intensity(std::to_string(intensity)), m_index(index)
		{}
		*/
		ViewerPointCloud()
			: m_x(0), m_y(0), m_z(0), m_intensity(0), m_index(0)
		{}
		ViewerPointCloud(float x, float y, float z, float intensity, int32_t index)
			: m_x(x), m_y(y), m_z(z),
			m_intensity(intensity), m_index(index)
		{}
		~ViewerPointCloud() = default;
		ViewerPointCloud(const ViewerPointCloud& vpc)
			: m_x(vpc.m_x)
			, m_y(vpc.m_y)
			, m_z(vpc.m_z)
			, m_intensity(vpc.m_intensity)
			, m_index(vpc.m_index)
		{}
		ViewerPointCloud(ViewerPointCloud&& vpc) noexcept
			: m_x(std::move(vpc.m_x))
			, m_y(std::move(vpc.m_y))
			, m_z(std::move(vpc.m_z))
			, m_intensity(std::move(vpc.m_intensity))
			, m_index(std::move(vpc.m_index))
		{}
		ViewerPointCloud& operator=(const ViewerPointCloud& vpc)
		{
			if (this != &vpc) {
				m_x = vpc.m_x;
				m_y = vpc.m_y;
				m_z = vpc.m_z;
				m_intensity = vpc.m_intensity;
				m_index = vpc.m_index;
			}

			return *this;
		}
		ViewerPointCloud& operator=(ViewerPointCloud&& vpc) noexcept
		{
			if (this != &vpc) {
				m_x = std::move(vpc.m_x);
				m_y = std::move(vpc.m_y);
				m_z = std::move(vpc.m_z);
				m_intensity = std::move(vpc.m_intensity);
				m_index = std::move(vpc.m_index);
			}

			return *this;
		}

	public:
		/*
		float getX() const { return std::stof(m_x); }
		float getY() const { return std::stof(m_y); }
		float getZ() const { return std::stof(m_z); }
		float getIntensity() const { return std::stof(m_intensity); }
		*/
		float getX() const { return m_x; }
		float getY() const { return m_y; }
		float getZ() const { return m_z; }
		float getIntensity() const { return m_intensity; }
		int32_t getIndex() const { return m_index; }

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version) {
			ar & m_x;
			ar & m_y;
			ar & m_z;
			ar & m_intensity;
			ar & m_index;
		}
	private:
		/*
		std::string m_x;
		std::string m_y;
		std::string m_z;
		std::string m_intensity;
		*/
		float m_x;
		float m_y;
		float m_z;
		float m_intensity;
		int32_t m_index;
	};

	class ViewerPointClouds {
	public:
		ViewerPointClouds()
			: m_pointClouds()
		{}
		ViewerPointClouds(const std::vector<ViewerPointCloud>& pointClouds)
			: m_pointClouds(pointClouds)
		{}
		virtual ~ViewerPointClouds() = default;
		ViewerPointClouds(const ViewerPointClouds& vpcs)
			: m_pointClouds(vpcs.m_pointClouds)
		{}
		ViewerPointClouds(ViewerPointClouds&& vpcs) noexcept
			: m_pointClouds(std::move(vpcs.m_pointClouds))
		{}
		ViewerPointClouds& operator=(const ViewerPointClouds& vpcs)
		{
			if (this != &vpcs) {
				m_pointClouds = vpcs.m_pointClouds;
			}
			
			return *this;
		}
		ViewerPointClouds& operator=(ViewerPointClouds&& vpcs) noexcept
		{
			if (this != &vpcs) {
				m_pointClouds = std::move(vpcs.m_pointClouds);
			}

			return *this;
		}
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version) {
			ar & m_pointClouds;
		}

	public:
		const std::vector<ViewerPointCloud>& getPointClouds() const { return m_pointClouds; }

	private:
		std::vector<ViewerPointCloud> m_pointClouds;
	};

	class ViewerObject {
	public:
		ViewerObject()
			: m_ID(0), m_label(0), m_score(0), m_position({ 0, 0, 0 }), m_dimension({ 0, 0, 0 })
		{}
		ViewerObject(int32_t id, uint32_t label, float score,
			const std::array<float, 3>& position, const std::array<float, 3>& dimension)
			: m_ID(id), m_label(label), m_score(score), m_position(position), m_dimension(dimension)
		{}
		ViewerObject(const ViewerObject& vo)
			: m_ID(vo.m_ID)
			, m_label(vo.m_label)
			, m_score(vo.m_score)
			, m_position(vo.m_position)
			, m_dimension(vo.m_dimension)
		{}
		ViewerObject(ViewerObject&& vo) noexcept
			: m_ID(std::move(vo.m_ID))
			, m_label(std::move(vo.m_label))
			, m_score(std::move(vo.m_score))
			, m_position(std::move(vo.m_position))
			, m_dimension(std::move(vo.m_dimension))
		{}
		ViewerObject& operator=(const ViewerObject& vo)
		{
			if (this != &vo) {
				m_ID = vo.m_ID;
				m_label = vo.m_label;
				m_score = vo.m_score;
				m_position = vo.m_position;
				m_dimension = vo.m_dimension;
			}

			return *this;
		}
		ViewerObject& operator=(ViewerObject&& vo) noexcept
		{
			if (this != &vo) {
				m_ID = std::move(vo.m_ID);
				m_label = std::move(vo.m_label);
				m_score = std::move(vo.m_score);
				m_position = std::move(vo.m_position);
				m_dimension = std::move(vo.m_dimension);
			}

			return *this;
		}

	public:
		int32_t getID() const { return m_ID; }
		uint32_t getLabel() const { return m_label; }
		float getScore() const { return m_score; }
		std::array<float, 3> getPosition() const { return m_position; }
		std::array<float, 3> getDimension() const { return m_dimension; }

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version) {
			ar & m_ID;
			ar & m_label;
			ar & m_score;
			ar & m_position;
			ar & m_dimension;
		}

	private:
		int32_t m_ID;
		uint32_t m_label;
		float m_score;
		std::array<float, 3> m_position;
		std::array<float, 3> m_dimension;
	};

	class ViewerObjects {
	public:
		ViewerObjects()
			: m_times(), m_objects()
		{}
		ViewerObjects(double times, const std::vector<ViewerObject>& objects)
			: m_times(times), m_objects(objects)
		{}
		virtual ~ViewerObjects() = default;
		ViewerObjects(const ViewerObjects& objs)
			: m_times(objs.m_times)
			, m_objects(objs.m_objects)
		{}
		ViewerObjects(ViewerObjects&& objs) noexcept
			: m_times(std::move(objs.m_times))
			, m_objects(std::move(objs.m_objects))
		{}
		ViewerObjects& operator=(const ViewerObjects& objs)
		{
			if (this != &objs) {
				m_times = objs.m_times;
				m_objects = objs.m_objects;
			}
			return *this;
		}
		ViewerObjects& operator=(ViewerObjects&& objs) noexcept
		{
			if (this != &objs) {
				m_times = std::move(objs.m_times);
				m_objects = std::move(objs.m_objects);
			}

			return *this;
		}

	public:
		double getTimes() const { return m_times; }
		std::vector<ViewerObject> getObjects() const { return m_objects; }
		
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version) {
			ar & m_times;
			ar & m_objects;
		}

	private:
		double m_times;
		std::vector<ViewerObject> m_objects;
	};

	class ViewerTrack {
	public:
		ViewerTrack()
			: m_ID(0), m_state({ 0, }), m_dimension({ 0, }), m_estimatedHistory()
		{}
		ViewerTrack(uint32_t id, const std::array<float, 6>& state,
			const std::array<float, 3>& dimension, const std::vector<std::array<float, 3>>& history)
			: m_ID(id), m_state(state), m_dimension(dimension), m_estimatedHistory(history)
		{}
		ViewerTrack(const ViewerTrack& vt)
			: m_ID(vt.m_ID)
			, m_state(vt.m_state)
			, m_dimension(vt.m_dimension)
			, m_estimatedHistory(vt.m_estimatedHistory)
		{}
		ViewerTrack(ViewerTrack&& vt) noexcept
			: m_ID(std::move(vt.m_ID))
			, m_state(std::move(vt.m_state))
			, m_dimension(std::move(vt.m_dimension))
			, m_estimatedHistory(std::move(vt.m_estimatedHistory))
		{}
		ViewerTrack& operator=(const ViewerTrack& vt)
		{
			if (this != &vt) {
				m_ID = vt.m_ID;
				m_state = vt.m_state;
				m_dimension = vt.m_dimension;
				m_estimatedHistory = vt.m_estimatedHistory;
			}

			return *this;
		}
		ViewerTrack& operator=(ViewerTrack&& vt) noexcept
		{
			if (this != &vt) {
				m_ID = std::move(vt.m_ID);
				m_state = std::move(vt.m_state);
				m_dimension = std::move(vt.m_dimension);
				m_estimatedHistory = std::move(vt.m_estimatedHistory);
			}

			return *this;
		}

	public:
		uint32_t getID() const { return m_ID; }
		std::array<float, 6> getState() const { return m_state; } // x, y, z, Vx, Vy, Vz
		std::array<float, 3> getDimension() const { return m_dimension; }
		std::vector<std::array<float, 3>> getEstimationHistory() const {
			return m_estimatedHistory;
		} // x, y, z

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version) {
			ar & m_ID;
			ar & m_state;
			ar & m_dimension;
			ar & m_estimatedHistory;
		}

	private:
		uint32_t m_ID;
		std::array<float, 6> m_state; // x, y, z, Vx, Vy, Vz
		std::array<float, 3> m_dimension;
		std::vector<std::array<float, 3>> m_estimatedHistory; // x, y, z
	};

	class ViewerTracks {
	public:
		ViewerTracks()
			: m_tracks()
		{}
		ViewerTracks(const std::vector<ViewerTrack>& tracks)
			: m_tracks(tracks)
		{}
		virtual ~ViewerTracks() = default;
		ViewerTracks(const ViewerTracks& vts)
			: m_tracks(vts.m_tracks)
		{}
		ViewerTracks(ViewerTracks&& vts) noexcept
			: m_tracks(std::move(vts.m_tracks))
		{}
		ViewerTracks& operator=(const ViewerTracks& vts)
		{
			if (this != &vts) {
				m_tracks = vts.m_tracks;
			}

			return *this;
		}
		ViewerTracks& operator=(ViewerTracks&& vts) noexcept
		{
			if (this != &vts) {
				m_tracks = std::move(vts.m_tracks);
			}

			return *this;
		}
	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version) {
			ar & m_tracks;
		}

	public:
		std::vector<ViewerTrack> getTracks() const { return m_tracks; }

	private:
		std::vector<ViewerTrack> m_tracks;
	};

	class ViewerData
	{
	public:
		ViewerData()
			: m_objects()
			, m_tracks()
			, m_pointClouds()
		{}
		ViewerData(const ViewerObjects& objects, const ViewerTracks& tracks,
			const ViewerPointClouds& pointClouds)
			: m_objects(objects)
			, m_tracks(tracks)
			, m_pointClouds(pointClouds)
		{}
		ViewerData(const ViewerData& vd)
			: m_objects(vd.m_objects)
			, m_tracks(vd.m_tracks)
			, m_pointClouds(vd.m_pointClouds)
		{}
		ViewerData(ViewerData&& vd) noexcept
			: m_objects(std::move(vd.m_objects))
			, m_tracks(std::move(vd.m_tracks))
			, m_pointClouds(std::move(vd.m_pointClouds))
		{}
		ViewerData& operator=(const ViewerData& vd)
		{
			if (this != &vd) {
				m_objects = vd.m_objects;
				m_tracks = vd.m_tracks;
				m_pointClouds = vd.m_pointClouds;
			}

			return *this;
		}
		ViewerData& operator=(ViewerData&& vd) noexcept
		{
			if (this != &vd) {
				m_objects = std::move(vd.m_objects);
				m_tracks = std::move(vd.m_tracks);
				m_pointClouds = std::move(vd.m_pointClouds);
			}

			return *this;
		}

	public:
		ViewerObjects getObjects() const { return m_objects; }
		ViewerTracks getTracks() const { return m_tracks; }
		ViewerPointClouds getPointClouds() const { return m_pointClouds; }

	private:
		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version) {
			ar & m_objects;
			ar & m_tracks;
			ar & m_pointClouds;
		}

	private:
		ViewerObjects m_objects;
		ViewerTracks m_tracks;
		ViewerPointClouds m_pointClouds;
	};
}